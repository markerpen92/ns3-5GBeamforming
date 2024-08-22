#include <iostream>
#include <deque>

#include "ns3/core-module.h"
#include "ns3/log.h"
#include "ns3/datatype.h"

#include "ns3/opengym-module.h"
#include "beamforming-agent.h"

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE("BeamformingAgent");



namespace ns3
{
    /*----------Initialize-------------------------------------------------------------------------*/
    ns3_ClassBF01_AP_BeamformingAgent::ns3_ClassBF01_AP_BeamformingAgent(
        uint32_t ObservationSize   , 
        uint32_t ActionSize        , 
        uint32_t RewardHistroySize , 
        uint32_t OpenGymPort       , 
        Time     TimeStep
    )
    {
        NS_LOG_FUNCTION(this);

        ObserveInterval = Seconds(TimeStep);
        cout << "Default Observe Interval : " << ObserveInterval << endl;
        Simulator::Schedule(
            Seconds(0.0) , 
            &ns3_ClassBF01_AP_BeamformingAgent::ScheduleStateTransition , 
            this
        );

        ns3_ClassBF01_AP_BeamformingAgent::ObservationSize = ObservationSize;
        ns3_ClassBF01_AP_BeamformingAgent::ActionSize      = ActionSize     ;
        ns3_ClassBF01_AP_BeamformingAgent::ObservationSpace.resize(ObservationSize);
        ns3_ClassBF01_AP_BeamformingAgent::ActionSpace     .resize(ActionSize     );
        ns3_ClassBF01_AP_BeamformingAgent::RewardHistory   .resize(RewardHistroySize);

        ns3_ClassBF01_AP_BeamformingAgent::OpenGymPort = OpenGymPort;
        ns3_ClassBF01_AP_BeamformingAgent::Agent       = CreateObject<OpenGymInterface>(
            ns3_ClassBF01_AP_BeamformingAgent::OpenGymPort
        );
    }

    void ns3_ClassBF01_AP_BeamformingAgent::Build()
    {
        ns3_ClassBF01_AP_BeamformingAgent::Agent->SetGetObservationSpaceCb(
            MakeCallback(&ns3_ClassBF01_AP_BeamformingAgent::CreateActionSpace , this)
        );
        ns3_ClassBF01_AP_BeamformingAgent::Agent->SetGetActionSpaceCb     (
            MakeCallback(&ns3_ClassBF01_AP_BeamformingAgent::CreateActionSpace , this)
        );
        ns3_ClassBF01_AP_BeamformingAgent::Agent->SetGetObservationCb     (
            MakeCallback(&ns3_ClassBF01_AP_BeamformingAgent::GetState        ,   this)
        );
        ns3_ClassBF01_AP_BeamformingAgent::Agent->SetExecuteActionsCb     (
            MakeCallback(&ns3_ClassBF01_AP_BeamformingAgent::ExecuteAction   ,   this)
        );
        ns3_ClassBF01_AP_BeamformingAgent::Agent->SetGetRewardCb          (
            MakeCallback(&ns3_ClassBF01_AP_BeamformingAgent::GetReward       ,   this)
        );
        ns3_ClassBF01_AP_BeamformingAgent::Agent->SetGetGameOverCb        (
            MakeCallback(&ns3_ClassBF01_AP_BeamformingAgent::IsDone            , this)
        );
        ns3_ClassBF01_AP_BeamformingAgent::Agent->SetGetExtraInfoCb       (
            MakeCallback(&ns3_ClassBF01_AP_BeamformingAgent::GetInfo           , this)
        );
    }
    /*---------------------------------------------------------------------------------------------*/

    /*------------Delete AP's Agent----------------------------------------------------------------*/
    ns3_ClassBF01_AP_BeamformingAgent::~ns3_ClassBF01_AP_BeamformingAgent()
    {
        NS_LOG_FUNCTION(this);
        cout << "Delete the agent(Work : Beamforming) in AP --- CodeName<" << this << ">" << endl;
    }
    /*---------------------------------------------------------------------------------------------*/



    /*--------Create Observation & Action Space-------------------------------------------------------------*/
    Ptr<OpenGymSpace> ns3_ClassBF01_AP_BeamformingAgent::CreateObservationSpace(void)
    {
        float low  = 0.0 ;
        float high = 10.0;
        vector<uint32_t> shape = {ns3_ClassBF01_AP_BeamformingAgent::ObservationSize};
        string ObservationType = TypeNameGet<double>();
        Ptr<OpenGymBoxSpace> ObservationSpace = CreateObject<OpenGymBoxSpace>(
            low , high , shape , ObservationType
        );
        return ObservationSpace;
    }

    Ptr<OpenGymSpace> ns3_ClassBF01_AP_BeamformingAgent::CreateActionSpace(void)
    {
        float low  = 0.0 ;
        float high = 10.0;
        vector<uint32_t> shape = {ns3_ClassBF01_AP_BeamformingAgent::ActionSize};
        string ObservationType = TypeNameGet<double>();
        Ptr<OpenGymBoxSpace> ActionSpace = CreateObject<OpenGymBoxSpace>(
            low , high , shape , ObservationType
        );
        return ActionSpace;
    }   
    /*---------------------------------------------------------------------------------------------*/



    /*---------Get State---------------------------------------------------------------------------*/
    Ptr<OpenGymDataContainer> ns3_ClassBF01_AP_BeamformingAgent::GetState()
    {
        ns3_ClassBF01_AP_BeamformingAgent::UpdateObservationSpace();
        vector<uint32_t> shape = {ns3_ClassBF01_AP_BeamformingAgent::ObservationSize};
        Ptr<OpenGymBoxContainer<double>> box = CreateObject<OpenGymBoxContainer<double>>(shape);

        for(uint32_t obs_idx=0 ; obs_idx<ns3_ClassBF01_AP_BeamformingAgent::ObservationSpace.size() ; obs_idx++)
        {
            box->AddValue(ns3_ClassBF01_AP_BeamformingAgent::ObservationSpace[obs_idx]);
        }

        return box;
    }

    void ns3_ClassBF01_AP_BeamformingAgent::UpdateObservationSpace()
    {
        static double* next_state = nullptr;
        if(!next_state)
        {
            cout << "<ns3-agent error> You didn't modify set the function to update new observation space"
            << endl;
            exit(1);
        }
        ns3_ClassBF01_AP_BeamformingAgent::ObservationSpace.push_front(*next_state);
        ns3_ClassBF01_AP_BeamformingAgent::ObservationSpace.pop_back  ();
    }
    /*---------------------------------------------------------------------------------------------*/



    /*---------Exe Action--------------------------------------------------------------------------*/
    bool ns3_ClassBF01_AP_BeamformingAgent::ExecuteAction(Ptr<OpenGymDataContainer> Action)
    {
        return true;
    }
    /*---------------------------------------------------------------------------------------------*/



    /*---------Get Reward--------------------------------------------------------------------------*/
    float ns3_ClassBF01_AP_BeamformingAgent::GetReward()
    {
        float *reward = nullptr;

        if(!reward)
        {
            cout << "You didn't define reward function" << endl;
            exit(1);
        }

        ns3_ClassBF01_AP_BeamformingAgent::Reward = *reward;
        return *reward;
    }
    /*---------------------------------------------------------------------------------------------*/



    /*---------Scheduling Event : State Transition-------------------------------------------------*/
    void ns3_ClassBF01_AP_BeamformingAgent::ScheduleStateTransition()
    {
        Simulator::Schedule(
            ns3_ClassBF01_AP_BeamformingAgent::ObserveInterval, 
            &ns3_ClassBF01_AP_BeamformingAgent::ScheduleStateTransition, 
            this
        );
        ns3_ClassBF01_AP_BeamformingAgent::Agent->NotifyCurrentState();
    }
    /*---------------------------------------------------------------------------------------------*/



    /*---------Agent Setting------------------------------------------------------------------------*/
    bool ns3_ClassBF01_AP_BeamformingAgent::IsDone()
    {
        return false;
    }

    string ns3_ClassBF01_AP_BeamformingAgent::GetInfo()
    {
        string str = "Not implement ns3_ClassBF01_AP_BeamformingAgent::GetInfo\n";
        return str;
    }
    /*----------------------------------------------------------------------------------------------*/



    /*----------Half Global variable----------------------------------------------------------------*/
    template <typename T> T ns3_ClassBF01_AP_BeamformingAgent::Find(string Key)
    {
        if(ns3_ClassBF01_AP_BeamformingAgent::INT_Variables.contains(Key))
        {
            int int_value = ns3_ClassBF01_AP_BeamformingAgent::INT_Variables[Key];
            return int_value;
        }
        if(ns3_ClassBF01_AP_BeamformingAgent::FLOAT_Variables.contains(Key))
        {
            float float_vlaue = ns3_ClassBF01_AP_BeamformingAgent::FLOAT_Variables[Key];
            return float_vlaue;
        }
        if(ns3_ClassBF01_AP_BeamformingAgent::DOUBLE_Variables.contains(Key))
        {
            double double_vlaue = ns3_ClassBF01_AP_BeamformingAgent::DOUBLE_Variables[Key];
            return double_vlaue;
        }
        if(ns3_ClassBF01_AP_BeamformingAgent::STRING_Variables.contains(Key))
        {
            string string_vlaue = ns3_ClassBF01_AP_BeamformingAgent::STRING_Variables[Key];
            return string_vlaue;
        }
    }
    /*----------------------------------------------------------------------------------------------*/
}
