#pragma warning( push )
#pragma warning( disable : 4245 )
#include <dds/dds.hpp>
#pragma warning( pop )

using namespace dds::core;
using namespace dds::core::policy;
using namespace dds::domain;
using namespace dds::domain::qos;
using namespace dds::topic;
using namespace dds::sub;
using namespace dds::sub::qos;
using namespace dds::sub::status;

#include "bus_info.h"

ParticipantBuiltinTopicDataListener::ParticipantBuiltinTopicDataListener(BusInfo* businfo)
        : _businfo(businfo)
    {

    }

 // NoOpDataReaderListener
void ParticipantBuiltinTopicDataListener::on_data_available(dds::sub::DataReader<dds::topic::TParticipantBuiltinTopicData<org::eclipse::cyclonedds::topic::ParticipantBuiltinTopicDataDelegate>>& reader)
{
    auto current_reader = reader;
    auto samples =
        current_reader.select() // reader
        .state(DataState::new_instance())
        .take();

    for (const auto & sample : samples)
    {
        if (!sample.info().valid())
        {
            continue;
        }
        // read participant name

        std::string name = "Participant";
        // TODO this won't compile with data() - ParticipantBuiltinTopicData does not seam to have a reead method?!
        auto user_data = sample;//data().user_data().value();

        /* std::string name(user_data.begin(), user_data.end());*/

        _businfo->onUserDataReceived(name);
    }
}


BusInfo::ParticipantInfo::ParticipantInfo(const std::string & participant_name) :
    _participant_name(participant_name)
{

}

bool BusInfo::ParticipantInfo::parse(const std::string & json)
{
    const auto rawJsonLength = static_cast<int>(json.length());
    JSONCPP_STRING err;
    Json::Value root;
    const Json::CharReaderBuilder builder;
    const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    if (!reader->parse(json.c_str(), json.c_str() + rawJsonLength, &root,
        &err))
    {
        return false;
    }

    _participant_name = root["participant_name"].asString();

    if (root["fep_version"].isObject())
    {
        _fep_version.major = root["fep_version"]["major"].asInt();
        _fep_version.minor = root["fep_version"]["minor"].asInt();
        _fep_version.patch = root["fep_version"]["patch"].asInt();
    }

    return true;
}

std::string BusInfo::ParticipantInfo::asJson() const
{
    Json::Value root;

    root["participant_name"] = _participant_name;

    Json::Value version;
    version["major"] = _fep_version.major;
    version["minor"] = _fep_version.minor;
    version["patch"] = _fep_version.patch;
    root["fep_version"] = version;

    Json::StreamWriterBuilder writer_builder;
    writer_builder["indentation"] = ""; // we want whitespace-less output
    return Json::writeString(writer_builder, root);
}

void BusInfo::ParticipantInfo::setParticipantName(const std::string& participant_name)
{
    _participant_name = participant_name;
}

void BusInfo::ParticipantInfo::setFepVersion(BusInfo::Version fep_version)
{
    _fep_version = fep_version;
}

std::string BusInfo::ParticipantInfo::getParticipantName() const
{
    return _participant_name;
}

BusInfo::Version BusInfo::ParticipantInfo::getFepVersion() const
{
    return _fep_version;
}


BusInfo::BusInfo():
    _listener(this)
{
    _own_participant_info = std::make_unique<BusInfo::ParticipantInfo>();
}

BusInfo::~BusInfo()
{
    //_listener_binder.reset();
}

void BusInfo::registerUserData(dds::domain::qos::DomainParticipantQos & qos)
{
    auto json = _own_participant_info->asJson();
    if (json.size() > 2048)
    {
        throw std::length_error("Internal error: max_participant_user_data are reached");
    }
    else
    {
        qos << dds::core::policy::UserData(dds::core::ByteSeq(json.begin(), json.end()));
    }
}

void BusInfo::registerParticipant(dds::domain::DomainParticipant & participant)
{
    // TODO register listener for builtin DataReader<ParticipantBuiltinTopicData>
    // to ParticipantBuiltinTopicDataListener with dds::core::status::StatusMask::data_available()
    // Problem: can't get builtin data reader as an object
}

void BusInfo::unregisterParticipant(dds::domain::DomainParticipant& /*participant*/)
{
    _callback = nullptr;
}

void BusInfo::onUserDataReceived(const std::string& user_data)
{
    std::unique_ptr< ParticipantInfo > participant_info = std::make_unique< ParticipantInfo >();

    if (participant_info->parse(user_data))
    {
        _participant_infos[participant_info->getParticipantName()] = std::move(participant_info);
        _callback();
    }
}


std::map<std::string, std::shared_ptr< BusInfo::ParticipantInfo >> BusInfo::getParticipantInfos() const
{
    return _participant_infos;
}

BusInfo::ParticipantInfo* BusInfo::getOwnParticipantInfo() const
{
    return _own_participant_info.get();
}

void BusInfo::setUpdateCallback(const std::function<void()>& callback)
{
    _callback = callback;
}

ParticipantBuiltinTopicDataListener BusInfo::getListener()
{
    return _listener;
}

std::string BusInfo::asJson() const
{
    std::string json = "[";

    bool first = true;

    for (const auto entry : _participant_infos)
    {

        if (first)
        {
            first = false;
        }
        else
        {
            json += ", ";
        }

        json += entry.second->asJson();

    };
    json += "]";

    return json;
}
