/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#include "ns3/packet.h"
#include "srnc.h"
#include "ns3/srnc-header.h"
#include "ns3/ipv4-header.h"
#include "ns3/srnc-tag.h"
#include "ns3/snr-tag.h"
#include "ns3/tcp-option-ts.h"
//#include "ns3/lte-pdcp-header.h"
#include "ns3/log.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-routing-protocol.h"
#include <deque>
#include <list>
#include "ns3/log.h"
#include <vector>
#include <algorithm>
#include <map>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "ns3/random-variable-stream.h"
#include "ns3/ipv4-header.h"
#include <bits/stdc++.h>//for rank computing
#include <iomanip>
#include "ns3/ppp-header.h"
#define R 160
#define C 160

#include <fstream>
using namespace ns3;
using namespace std;


namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("SrncProcessor");

SrncProcessor::SrncProcessor()
  : PduBuffer (0),
    PduBufferCount (0),
    StoreLabel (0),
    m_rnti (0)
{
  NS_LOG_FUNCTION (this);
}

SrncProcessor::~SrncProcessor()
{
  NS_LOG_FUNCTION (this);
}

TypeId
SrncProcessor::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::SrncProcessor")
    .SetParent<Object> ()
    .AddAttribute ("StoreLabel",
                   "Newtwork Coding Store Label",
                   UintegerValue (0),
                   MakeUintegerAccessor (&SrncProcessor::SetStoreLabel,
                                         &SrncProcessor::GetStoreLabel),
                   MakeUintegerChecker<uint32_t> ());
  return tid;
}

void
SrncProcessor::SetRnti (uint16_t rnti)
{
  //NS_LOG_FUNCTION (this << (uint32_t) rnti);
  m_rnti = rnti;
}

uint16_t
SrncProcessor::GetRnti ()
{
  return m_rnti;
}

void
SrncProcessor::Enqueue(Ptr<Packet> PdcpPdu)
{
  NS_LOG_FUNCTION (this);
  PduBuffer.push_back(PdcpPdu);
  PduBufferCount++;
  NS_LOG_INFO("PdcpPdu Buffer Count " << PduBufferCount << " PdcpPdu " << PdcpPdu);
}

void
SrncProcessor::ClearEnqueueBuffer()
{
  NS_LOG_FUNCTION (this);
  PduBuffer.clear();
  PduBufferCount = 0;
  NS_LOG_INFO("PdcpPdu Buffer Count after clear " << PduBufferCount);
}

uint32_t
SrncProcessor::GetEnqueueBufferSize()
{
  //NS_LOG_FUNCTION (this);
  //NS_LOG_INFO("PdcpPdu Buffer size " << PduBuffer.size());
  return PduBuffer.size();
}

void
SrncProcessor::SetStoreLabel(uint32_t storelabel)
{
  NS_LOG_FUNCTION (this);
  StoreLabel = storelabel;
  NS_LOG_INFO("SRNC encode packet with Label " << StoreLabel);
}

uint32_t
SrncProcessor::GetStoreLabel(void) const
{
  NS_LOG_FUNCTION (this);
  return StoreLabel;
}

std::list<Ptr<Packet>>
SrncProcessor::NetworkCoding(int listseq)
{
  std::list<Ptr<Packet>> NCodedBuffer;
  SrncHeader srncheader;
  PppHeader ppHD;
  Ipv4Header iHD;
  TcpHeader tHD;
  // uint8_t NCProtocol=253;
  uint8_t ttl=100;
  uint8_t pri = 2*uint8_t(listseq)+1;
  Ptr<Packet> TmpPacket;
  for (uint32_t i = 0; i < PduBufferCount; i++){
    GenerateNetworkCodingHeader(&iHD, StoreLabel, PduBufferCount, pri, i);
    TmpPacket = PduBuffer.at(i);  //高低優先權的編碼是使用相同的原始packet pointer
    Ptr<Packet> EncodedPacket = TmpPacket -> Copy();//但放到各自的buffer中要用新的pointer 否則傳送時會有問題(不同link傳輸時packet pointer會加上X2 header)
    EncodedPacket->RemoveHeader(ppHD);
    EncodedPacket->RemoveHeader(iHD);
    EncodedPacket->AddHeader(srncheader);
    // iHD.SetProtocol(NCProtocol);
    iHD.SetTtl (ttl);
    EncodedPacket->AddHeader(iHD);
    EncodedPacket->AddHeader(ppHD);
    NCodedBuffer.push_back(EncodedPacket);
  }

  return NCodedBuffer;
}

std::list<Ptr<Packet>>
SrncProcessor::NetworkCodingRedundancy(uint32_t availablenumber,int listseq)
{
  std::list<Ptr<Packet>> NCodedBufferRedundancy;
  SrncHeader srncheader;
  PppHeader ppHD;
  Ipv4Header iHD;
  TcpHeader tHD;
  // uint8_t NCProtocol=253;
  uint8_t ttl=100;
  uint8_t pri = 2*uint8_t(listseq);
  Ptr<Packet> TmpPacket;
  if (availablenumber > PduBuffer.size())
  {
    availablenumber = PduBuffer.size();
  }
  if(PduBufferCount > 0){
    for (uint32_t i = 0; i < availablenumber; i++){
      GenerateNetworkCodingHeader(&iHD, StoreLabel, PduBufferCount, pri, i);
      TmpPacket = PduBuffer.at(i);//統一都用第一個封包來暫時編碼,反正最後恢復封包是靠tag
      Ptr<Packet> EncodedPacket = TmpPacket -> Copy();//
      EncodedPacket->RemoveHeader(ppHD);
      EncodedPacket->RemoveHeader(iHD);
      EncodedPacket->AddHeader(srncheader);
      // iHD.SetProtocol(NCProtocol);
      iHD.SetTtl (ttl);
      EncodedPacket->AddHeader(iHD);
      EncodedPacket->AddHeader(ppHD);
      NCodedBufferRedundancy.push_back(EncodedPacket);
    }
  }
  return NCodedBufferRedundancy;
}

void
SrncProcessor::GenerateNetworkCodingHeader(Ipv4Header* iHD, uint32_t label, uint8_t encodednumber, uint8_t pri, uint8_t ith)
{
  NS_LOG_FUNCTION (this);
  std::vector<uint8_t> CoeffVector;
  iHD->SetEncodedLabel(label);
  iHD->SetEncodedPacketNumber(encodednumber);
  iHD->SetPri(pri);
  if(pri%2 == 1){
    for(uint8_t i = 0; i < 14; i++){
        // if(ith == i){//第幾個編碼封包則第幾個係數為1
        //     CoeffVector.push_back(1);
        // }
        // else{
        //     CoeffVector.push_back(0);
        // }
      if (i < encodednumber )
       {
        uint8_t x = rand()%255+1;
        CoeffVector.push_back(x);
       }
       else
       {
        CoeffVector.push_back(0);
       }
    }
    // CoeffVector.push_back(ith);
    iHD->SetEncodedCoeffVector(CoeffVector);
  }
  else{
    for(uint8_t j = 0; j < 14; j++){
       if (j < encodednumber )
       {
        uint8_t x = rand()%255+1;
        CoeffVector.push_back(x);
       }
       else
       {
        CoeffVector.push_back(0);
       }
    }
    // CoeffVector.push_back(ith);
    iHD->SetEncodedCoeffVector(CoeffVector);
  }
  CoeffVector.clear();
  // NS_LOG_INFO("SRNC header" << 
  // " Label " << srncheader->GetEncodedLabel() << 
  // " Encoded Number " << (uint32_t)srncheader->GetEncodedPacketNumber() << 
  // " priority " << (uint32_t)srncheader->GetPri());
  
  // std::vector <uint8_t> TmpVector;
  // char tmpcoeff[encodednumber];
  // TmpVector = iHD->GetEncodedCoeffVector();
  // for(int i = 0; i < encodednumber; i++){
  //   tmpcoeff[i] = TmpVector[i] + '0';
  //   std::cout << "SRNC header coefficient = " << tmpcoeff[i]<<"\n";
  // }
  // NS_LOG_INFO("SRNC header coefficient " << tmpcoeff);

}

int
SrncProcessor::LookUpPrioInBuffer(bool incomingPrio)
{
  NS_LOG_FUNCTION (this);
  int FindResult = -1;
  SrncHeader srncheader;
  Ptr<Packet> p;
  for(int i = PduBuffer.size() - 1; i >= 0; i--){
    p = PduBuffer.at(i);
    p->PeekHeader (srncheader);
    if((uint8_t)incomingPrio == srncheader.GetPri()){
      FindResult = i;
      break;
    }
  }
  return FindResult;
}

void
SrncProcessor::ReplaceEnqueue(Ptr<Packet> PdcpPdu,uint32_t replaceIndex)
{
  NS_LOG_FUNCTION (this);
  PduBuffer[replaceIndex] = PdcpPdu;
}

void
SrncProcessor::DiscardAllPacketWithLabel(uint32_t discardlabel)
{
  NS_LOG_FUNCTION (this << " discard label " << (int)discardlabel);
  NS_LOG_INFO("buffer size before discard packet " << PduBuffer.size());
  // std::cout << PduBuffer.size() << "\n";
  for(std::vector<Ptr<Packet>>::iterator it = PduBuffer.begin();
      it != PduBuffer.end();) {
    PppHeader ppHD;
    Ipv4Header iHD;
    auto PacketIterator = *it;
    if(PacketIterator -> GetSize() == 1454){  //由於在DecodeAllWithLabel已經remove掉NC header 所以用大小判斷
       it = PduBuffer.erase(it);
     }
    else{
      ++it;
    }
  }
  // std::cout << "Discard label : " << discardlabel << " ; now LeftBuffer size = " << PduBuffer.size() << "\n";
  NS_LOG_INFO("buffer size after discard packet " << PduBuffer.size());
}

void
SrncProcessor::DiscardAllPacketWithLabel2(uint32_t discardlabel)
{
  NS_LOG_FUNCTION (this << " discard label " << (int)discardlabel);
  NS_LOG_INFO("buffer size before discard packet " << PduBuffer.size());

  for(std::vector<Ptr<Packet>>::iterator it = PduBuffer.begin();
      it != PduBuffer.end();) {
    PppHeader ppHD;
    Ipv4Header iHD;
    auto PacketIterator = *it;
    PacketIterator->RemoveHeader(ppHD);
    PacketIterator->PeekHeader (iHD);
    if(iHD.GetEncodedLabel() == discardlabel){  //由於在DecodeAllWithLabel已經remove掉NC header 所以用大小判斷
       it = PduBuffer.erase(it);
     }
    else{
      PacketIterator->AddHeader(ppHD);
      ++it;
    }
  }
  // std::cout << "Discard label : " << discardlabel << " ; now LeftBuffer size = " << PduBuffer.size() << "\n";
  NS_LOG_INFO("buffer size after discard packet " << PduBuffer.size());
}

uint32_t
SrncProcessor::FindPacketNumberWithLabel(uint32_t label)
{
  NS_LOG_FUNCTION (this);
  uint32_t PacketNumbers = 0;
  for(std::vector<Ptr<Packet>>::iterator it = PduBuffer.begin();
      it != PduBuffer.end();) {
    PppHeader ppHD;
    Ipv4Header iHD;
    auto PacketIterator = *it;
    PacketIterator->RemoveHeader(ppHD);
    PacketIterator->PeekHeader(iHD);
    if(iHD.GetEncodedLabel() == label){
      PacketNumbers++;
    }
    PacketIterator->AddHeader(ppHD);
    ++it;
  }
  return PacketNumbers;
}

uint8_t
SrncProcessor::FindPacketEncodedNumberWithLabel(uint32_t label)
{
  NS_LOG_FUNCTION (this);
  uint8_t PacketEncodedNumber = 0;
  for(std::vector<Ptr<Packet>>::iterator it = PduBuffer.begin();
      it != PduBuffer.end();) {
    PppHeader ppHD;
    Ipv4Header iHD;
    auto PacketIterator = *it;
    PacketIterator->RemoveHeader(ppHD);
    PacketIterator->PeekHeader(iHD);

    if(iHD.GetEncodedLabel() == label){
      PacketEncodedNumber = iHD.GetEncodedPacketNumber();
      PacketIterator->AddHeader(ppHD);
      break;
    }
    PacketIterator->AddHeader(ppHD);
    ++it;
  }
  return PacketEncodedNumber;
}

uint32_t
SrncProcessor::FindSmallestLabelInBuffer()
{
  NS_LOG_FUNCTION (this);
  uint32_t smallestlabel = 4294967294;
  for(std::vector<Ptr<Packet>>::iterator it = PduBuffer.begin();
      it != PduBuffer.end();) {
    SrncHeader srncheader;
    PppHeader ppHD;
    Ipv4Header iHD;
    TcpHeader tHD;
    auto PacketIterator = *it;
    if (PacketIterator -> GetSize() > 1454)
    {
      PacketIterator->RemoveHeader(ppHD);
      // PacketIterator->RemoveHeader(iHD);
      // PacketIterator->PeekHeader(srncheader);
      // PacketIterator->AddHeader(iHD);
      PacketIterator->PeekHeader(iHD);
      PacketIterator->AddHeader(ppHD);
      if(smallestlabel > iHD.GetEncodedLabel()){
         smallestlabel = iHD.GetEncodedLabel();
      }
    }
    ++it;
  }
  return smallestlabel;
}
// int pickarr[10] = {3,6,9,10,1,4,7,2,5,8};
std::vector<uint32_t> DecodefailBuffer;
Ptr<Packet> packettag;
std::vector<Ptr<Packet>>
SrncProcessor::DecodeAllWithLabel(uint32_t decodedlabel)
{
  NS_LOG_FUNCTION (this);

  uint16_t HighPrioCount = 0, LowPrioCount = 0;
  std::vector <uint8_t> ReceivedPrioVector;
  int rank = 0;
  uint8_t encodednumber = 0;
  std::vector<Ptr<Packet>> DecodePacketBuffer;
  std::vector<Ptr<Packet>> OutputDecodePacketBuffer;


  // get all NC packet with decodedlabel for decoding (DecodePacketBuffer)
  // 找出這個要decode的所有封包,並放進DecodePacketBuffer
  for(std::vector<Ptr<Packet>>::iterator it = PduBuffer.begin();
      it != PduBuffer.end();) {
    PppHeader ppHD;
    Ipv4Header iHD;
    auto PacketIterator = *it;
    PacketIterator->RemoveHeader(ppHD);
    PacketIterator->PeekHeader(iHD);
    if(decodedlabel == iHD.GetEncodedLabel()){
      if(iHD.GetPri()){
        HighPrioCount++;}
      else{
        LowPrioCount++;}
        DecodePacketBuffer.push_back(PacketIterator);//目前沒有宣告新的packet pointer,no PacketIterator->Copy()
        encodednumber = iHD.GetEncodedPacketNumber();
    }
    ++it;
    PacketIterator->AddHeader(ppHD);
  }
   // std::cout << "DecodePacketBuffer.size() = " << DecodePacketBuffer.size() << "\n";

  // 如果buffer裡有東西就代表要decode了
  if(DecodePacketBuffer.size() != 0)
    {
    // NS_LOG_INFO(" find " << DecodePacketBuffer.size() << " packet with label " << decodedlabel << " for decoding");
    // int **DecodingCoeff, z = 0, b = 0;
    // DecodingCoeff = new int*[R];
    // for(z=0;z<R;z++){
    //   DecodingCoeff[z] = new int[C];
    // }
    // for(z=0;z<R;z++){
    //   for(b=0;b<C;b++){
    //     DecodingCoeff[z][b] = 0;
    //   }
    // }
    uint8_t j = 0;
    //get all decoding coefficient in DecodePacketBuffer
    // 用來取得coefficient
    for(std::vector<Ptr<Packet>>::iterator it2 = DecodePacketBuffer.begin();
        it2 != DecodePacketBuffer.end();) {
      PppHeader ppHD;
      Ipv4Header iHD;
      TcpHeader tHD;
      SrncHeader srncheader;

      auto PacketIterator2 = *it2;
      PacketIterator2->RemoveHeader(ppHD);
      
      // PacketIterator2->PeekHeader(srncheader);//Don't remove NC header. 否則要丟棄封包時會找不到SRNC header的特定label
      // PacketIterator2->RemoveHeader(srncheader);
      PacketIterator2->PeekHeader(iHD);
      // if(iHD.GetPri()){
      //   HighPrioCount++;
      // }else{
      //   LowPrioCount++;
      // }
      // std::vector <uint8_t> tmpcoeff = iHD.GetReceivedHeaderCoeffVector();
      // for(uint8_t i = 0; i < encodednumber; i++){
      //   DecodingCoeff[j][i] = (int)tmpcoeff[i];
        //NS_LOG_INFO(" after " << DecodingCoeff[j][i] << " before " << tmpcoeff[i] << (int)j << " " << (int)i);
      // }
      PacketIterator2->RemoveHeader(iHD);
      uint8_t ttl=60;
      iHD.SetTtl (ttl);
      uint8_t TcpProtocol = 6;
      iHD.SetProtocol(TcpProtocol);
      ReceivedPrioVector.push_back(iHD.GetPri());
      PacketIterator2->RemoveHeader(srncheader);
      PacketIterator2->AddHeader(iHD);
      PacketIterator2->AddHeader(ppHD);
      j++;
      ++it2;
    }
    //calculate rank of decoding coefficient
    // 檢查rank
    // rank = findRank(DecodingCoeff, R, C);
    rank = HighPrioCount + LowPrioCount;
    // for(z=0;z<R;z++){
    //   delete [] DecodingCoeff[z];
    // }
    // delete [] DecodingCoeff;

    //recover seq and remove tag
    // 宣告待會要用的srncTag(用於恢復seq)
    SrncTag srncTag;
    srncTag.SetUseUdp(0);//如果沒設這個會少Serialize/Deserialize tcp sequence
    Ptr<Packet> tmp = DecodePacketBuffer.at(0);
    tmp->FindFirstMatchingByteTag(srncTag);
    std::vector <uint32_t> tcpSeqVector = srncTag.GetTcpSeqVector();

    // std::cout << "tcpSeqVector = " << tcpSeqVector.size() << "\n";
    // std::vector <uint32_t> tcpTsVector = srncTag.GetTcpTsVector();
    // std::vector <uint32_t> tcpEhVector = srncTag.GetTcpEhVector();
  
    if(rank >= encodednumber)
    {
      // std::cout << "rank = " << rank << "\n";
        for(uint8_t i = 0; i < encodednumber; i++) //只要恢復跟encodednumber一樣數量的封包就好
        {
          // std::cout << "now dealwith pkt = " << i <<"\n";
          packettag = DecodePacketBuffer.at(i);
          PppHeader ppHD;
          Ipv4Header iHD;
          TcpHeader tHD;
          packettag->RemoveHeader (ppHD);
          packettag->RemoveHeader (iHD);
          packettag->RemoveHeader (tHD);

          SequenceNumber32 sequencenumber32 (tcpSeqVector[i]);
          // std::cout << "tcpSeqVector = " << (uint32_t)tcpSeqVector[i] << "\n";
          tHD.SetSequenceNumber(sequencenumber32);

          packettag->AddHeader (tHD);
          packettag->AddHeader (iHD);
          packettag->AddHeader (ppHD);
          // std::cout << "packettag size = " << packettag -> GetSize() << "\n";
          OutputDecodePacketBuffer.push_back(packettag);
      }

      NS_LOG_DEBUG("At " << Simulator::Now().GetNanoSeconds() << " Decoding success with label " << decodedlabel << " rank " << rank <<  " encodednumber " << (int)encodednumber << " rnti" << m_rnti);
      // std::cout << std::setw(6) << "[SRNC]" << std::setw(8) << "[Wigig]" << std::setw(5) << "[User]" << "[" << this << "]" 
      //           << "[" << std::setw(10) << Simulator::Now ().GetSeconds() << "]"
      //           << " find " << DecodePacketBuffer.size() << " decoding success " << decodedlabel
      //           << " HighPrioCount " << HighPrioCount << " LowPrioCount " << LowPrioCount
      //           << " rank " << rank << " encodednumber " << (int)encodednumber << " ReceivedPrioVector ";
      // for(uint16_t i = 0; i < ReceivedPrioVector.size(); i++)
      // {
      //   std::cout << (int)ReceivedPrioVector[i] << " ";
      //     ofstream fout;
      //     fout.open( "0_Decode_lable.txt" ,ios::app);
      //     fout << Simulator::Now ().GetSeconds ()  <<  " "  << (int)ReceivedPrioVector[i] << "\n";
      //     fout.close();
      // }
      // std::cout << "\n";
      // for(uint16_t i = 0; i < ReceivedPrioVector.size(); i++)
      // {
      //   ofstream fout;
      //   fout.open( "0_Decode_lable.txt" ,ios::app);
      //   fout << Simulator::Now ().GetSeconds ()  <<  " "  << (int)ReceivedPrioVector[i] << "\n";
      //   fout.close();
      // }

      DecodePacketBuffer.clear();
    }
    else
    {  //rank < encodeNumber
      DecodefailBuffer.push_back(decodedlabel);
      for(uint8_t i = 0; i < HighPrioCount; i++)  // 解不回來只送High
        {
          packettag = DecodePacketBuffer.at(i);
          OutputDecodePacketBuffer.push_back(packettag);
      }
      NS_LOG_DEBUG("At " << Simulator::Now().GetNanoSeconds() << " Decoding fail with label " << decodedlabel << " rank " << rank << " encodednumber " << (int)encodednumber << " rnti" << m_rnti);
      std::cout << std::setw(6) << "[SRNC]" << std::setw(8) << "[Wigig]" << std::setw(5) << "[User]" << "[" << this << "]" 
                << "[" << std::setw(10) << Simulator::Now ().GetSeconds() << "]"
                << " find " << DecodePacketBuffer.size() << " decoding fail " << decodedlabel
                << " HighPrioCount " << HighPrioCount << " LowPrioCount " << LowPrioCount
                << " rank " << rank << " encodednumber " << (int)encodednumber << " ReceivedPrioVector ";
      for(uint16_t i = 0; i < ReceivedPrioVector.size(); i++){
        std::cout << (int)ReceivedPrioVector[i] << " ";
      }
      std::cout << "\n";
      DecodePacketBuffer.clear();
    }

  }
  else
  {
    NS_LOG_INFO("can't find any packet in buffer with label " << decodedlabel << " for decoding");
  }
    if (Simulator::Now().GetSeconds() > 2.0)
    { 
      std::vector<uint32_t>::iterator eraseit;
      std::sort(DecodefailBuffer.begin(), DecodefailBuffer.end());
      eraseit = std::unique(DecodefailBuffer.begin(), DecodefailBuffer.end());
      DecodefailBuffer.erase(eraseit, DecodefailBuffer.end());
    //   for(uint16_t k = 0; k < DecodefailBuffer.size(); k++){
    //      std::cout << (int)DecodefailBuffer[k] << " ";
    //   }
    // std::cout << "\n";
    }
  // std::cout << "OutputDecodePacketBuffer.size() = " << OutputDecodePacketBuffer.size() << "\n";
  return OutputDecodePacketBuffer;
}

std::vector<Ptr<Packet>>
SrncProcessor::BufferManagementOutput(void)
{
  std::vector<Ptr<Packet>> OutputPacketBuffer;
    for(std::vector<Ptr<Packet>>::iterator it = PduBuffer.begin(); it != PduBuffer.end();) 
    {
      auto PacketIterator = *it;
      OutputPacketBuffer.push_back(PacketIterator);
      ++it;
    }
      return OutputPacketBuffer;
}


int
SrncProcessor::rankOfMatrix(int mat[R][C])
{
  int rank = C; 

  for (int row = 0; row < rank; row++) 
  { 
      if (mat[row][row]) 
      { 
          for (int col = 0; col < R; col++) 
          { 
              if (col != row) 
              { 
                double mult = (double)mat[col][row] / 
                                      mat[row][row]; 
                for (int i = 0; i < rank; i++) 
                  mat[col][i] -= mult * mat[row][i]; 
            } 
          } 
      } 

      else
      { 
          bool reduce = true; 
          for (int i = row + 1; i < R;  i++) 
          { 
              if (mat[i][row]) 
              { 
                  swap(mat, row, i, rank); 
                  reduce = false; 
                  break ; 
              } 
          } 

          if (reduce) 
          { 
              rank--; 

              for (int i = 0; i < R; i ++) 
                  mat[i][row] = mat[i][rank]; 
          } 

          row--; 
      } 
  } 
  return rank; 
}

void
SrncProcessor::swap(int mat[R][C], int row1, int row2, int col)
{
  for (int i = 0; i < col; i++) 
  { 
      int temp = mat[row1][i]; 
      mat[row1][i] = mat[row2][i]; 
      mat[row2][i] = temp; 
  } 
}

int
SrncProcessor::findRank(int **arr,int row,int col)
{
    int rank = std::min(row,col);
    int pivot1,pivot2,i,j,k;
    
    for(i=0;i<rank;i++)
    {
        if(arr[i][i]!=0)
        {
            for(j=i+1;j<row;j++)
            {
                if(arr[j][i]!=0)
                {
                    pivot1 = arr[i][i];
                    pivot2 = arr[j][i];
                    for(k=i;k<col;k++)
                    {
                        arr[j][k] = arr[j][k]*pivot1 - arr[i][k]*pivot2;
                    }
                }
            }
        }
        else
        {
            int flag=1;
            for(j=i+1;j<row;j++)
            {
                if(arr[j][i])
                {
                    flag=0;
                    swapRows(arr,row,col,i,j);
                    break;
                }
            }
            if(flag)
            {
                rank--;
                swapCols(arr,row,col,i,col-1);
                i--;    //process this row again...important
            }
        }
    }
    /*for(i=0;i<row;i++)
    {
        for(j=0;j<col;j++)
        {
            cout<<arr[i][j]<<" ";
        }
        cout<<endl;
    }*/

    return rank;
}
void
SrncProcessor::swapRows(int **arr,int row,int col,int n,int m)
{
    int i;
    for(i=0;i<col;i++)
        std::swap(arr[n][i],arr[m][i]);
}

void
SrncProcessor::swapCols(int **arr,int row,int col,int n,int m)
{
    int i;
    for(i=0;i<row;i++)
        std::swap(arr[i][n],arr[i][m]);
}

uint8_t
SrncProcessor::GF256Mult(uint8_t a, uint8_t b)
{
  if ( a == 0 || b == 0)
    return 0;

  uint16_t log_res = ((uint16_t)gf256_log_table[a])
    + ((uint16_t)gf256_log_table[b]);
  log_res = (log_res + (log_res >> 8)) & 0xff; /* mod 255 */

  //NS_LOG_INFO("GF256Mult " << a << " " << b << " "<< (int)gf256_exp_table[log_res]);
  return gf256_exp_table[log_res];
}

uint8_t
SrncProcessor::GF256Add(uint8_t a, uint8_t b)
{
  return a ^ b;
}

uint8_t
SrncProcessor::GF256Div(uint8_t a, uint8_t b)
{
  if (a == 0)
    return 0;
  else if (b == 0){
    NS_LOG_INFO("GF256Div error b = " << b);
    return 0;
  }

  uint16_t log_res = ( 255 + (uint16_t)gf256_log_table[a])
    - ((uint16_t)gf256_log_table[b]);
  log_res = (log_res + (log_res >> 8)) & 0xff; /* mod 255 */

  //NS_LOG_INFO("GF256Div " << (int)a << " " << (int)b << " " << (int)gf256_exp_table[log_res]);
  return gf256_exp_table[log_res];
}


}

