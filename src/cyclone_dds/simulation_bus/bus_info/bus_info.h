

#pragma once

#include <json/json.h>
#include <dds/dds.hpp>
#include <map>

class BusInfo;

class ParticipantBuiltinTopicDataListener
    : public virtual dds::sub::NoOpDataReaderListener<dds::topic::ParticipantBuiltinTopicData>
{
public:
    ParticipantBuiltinTopicDataListener(BusInfo* businfo);

public: // NoOpDataReaderListener
    void on_data_available(dds::sub::DataReader<dds::topic::ParticipantBuiltinTopicData>& reader) override;
    
private:
    BusInfo* _businfo;
};

/*
* Collect and provide bus information via build_in topic from conext dds
*/
class BusInfo
{
public:
    struct Version
    {
        int major = 0;
        int minor = 0;
        int patch = 0;
    };

    class ParticipantInfo
    {
    public:
        ParticipantInfo() = default;
        ParticipantInfo(const std::string & participant_name);
        ~ParticipantInfo() = default;

        bool parse(const std::string & json);
        std::string asJson() const;

        std::string getParticipantName() const;
        Version getFepVersion() const;

        void setParticipantName(const std::string& participant_name);
        void setFepVersion(Version version);

    private:
        std::string _participant_name;
        Version _fep_version;
    };


public:
    BusInfo();
    ~BusInfo();

    std::map<std::string, std::shared_ptr< ParticipantInfo > > getParticipantInfos() const;
    ParticipantInfo* getOwnParticipantInfo() const;

    void registerParticipant(dds::domain::DomainParticipant & participant);
    void unregisterParticipant(dds::domain::DomainParticipant& participant);

    void registerUserData(dds::domain::qos::DomainParticipantQos & qos);

    void setUpdateCallback(const std::function<void()>& callback);

    std::string asJson() const;

    void onUserDataReceived(const std::string& user_data);

    ParticipantBuiltinTopicDataListener getListener();

private:
    std::map<std::string, std::shared_ptr< ParticipantInfo>> _participant_infos;
    std::shared_ptr<ParticipantInfo> _own_participant_info;
    std::function<void()> _callback;
    ParticipantBuiltinTopicDataListener _listener;
};
