#ifndef NS3_CLASSBF01_AP_BEAMFORMINGAGENT_H
#define NS3_CLASSBF01_AP_BEAMFORMINGAGENT_H

#include <iostream>
#include <deque>

#include "ns3/core-module.h"
#include "ns3/log.h"
#include "ns3/datatype.h"
#include "ns3/opengym-module.h"

using namespace ns3;
using namespace std;

namespace ns3 {

    class ns3_ClassBF01_AP_BeamformingAgent
    {
        /*--------------Declare Public attributes-------------------------------------------------*/    
        public : 
            /*===       AP Init       ===*/
            Ptr<OpenGymInterface> Agent;
            ns3_ClassBF01_AP_BeamformingAgent( // create agents2(now is multi-agents)
                uint32_t ObservationSize   , 
                uint32_t ActionSize        , 
                uint32_t RewardHistroySize , 
                uint32_t OpenGymPort       ,
                Time     TimeStep
            ); 
            void Build();

            /*===Agent create Observe&Action===*/
            virtual                   ~ns3_ClassBF01_AP_BeamformingAgent(    ); // delete agents
            Ptr<OpenGymSpace>         CreateObservationSpace            (void);
            Ptr<OpenGymSpace>         CreateActionSpace                 (void);

            /*===Agent get State&Action&Reward===*/
            Ptr<OpenGymDataContainer> GetState();
            bool   ExecuteAction(Ptr<OpenGymDataContainer> Action);
            float  GetReward  ();

            /*===   Agent Setting   ===*/
            bool   IsDone       (); // to check is the experiment is finish or not
            string GetInfo      (); // get the info you want from agent

            /*=== Half Global variable===*/
            Dict<string , int   > INT_Variables    = dict<string , int>   ({{"default", 0  }});
            Dict<string , double> DOUBLE_Variables = dict<string , double>({{"default", 0.0}});
            Dict<string , float > FLOAT_Variables  = dict<string , float> ({{"default", 0.0}});
            Dict<string , string> STRING_Variables = dict<string , string>({{"default", "0"}});
            template <typename T> T Find(string Key);

        /*--------------Declare Private attributes-------------------------------------------------*/
        private : 
            /*===   AP's Agent   ===*/
            uint32_t OpenGymPort = 5555;

            /*===Agent's Observe===*/
            void ScheduleStateTransition(); // schedule into simulator & agents.observe
            void UpdateObservationSpace (); // get next state and update observation space
            Time ObserveInterval;           // Schdeule Interval & Observe Intervel

            /*===Agent's Setting===*/
            bool             CopyPacket      = false;
            uint32_t         ObservationSize = 2    ;
            uint32_t         ActionSize      = 2    ;
            float            Reward          = 0.0  ;
            deque   <double> ObservationSpace       ;
            vector<uint32_t> ActionSpace            ;
            vector<double  > RewardHistory          ;
    };
    /*---------------------------------------------------------------------------------------------*/

} // namespace ns3

#endif // NS3_CLASSBF01_AP_BEAMFORMINGAGENT_H
