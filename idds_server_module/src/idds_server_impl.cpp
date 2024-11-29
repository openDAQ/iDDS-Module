#include <idds_server_module/idds_server_impl.h>
#include <coretypes/impl.h>
#include <coreobjects/property_object_factory.h>
#include <coreobjects/property_factory.h>
#include <opendaq/server_type_factory.h>
#include <opendaq/custom_log.h>
#include <iostream>

BEGIN_NAMESPACE_OPENDAQ_IDDS_SERVER_MODULE

using namespace daq;

iDDSServerImpl::iDDSServerImpl(const DevicePtr& rootDevice,
                               const PropertyObjectPtr& config,
                               const ContextPtr& context)
    : Server("OpenDAQiDDS", config, rootDevice, context)
    , iDDSServer("iDDSServer1")
    , context(context)
{
    iDDSServer.StartServer();
}

iDDSServerImpl::~iDDSServerImpl()
{
}

ServerTypePtr iDDSServerImpl::createType()
{
    return ServerType(
        "OpenDAQiDDS",
        "iDDS",
        "iDDS server module");
}

void iDDSServerImpl::onStopServer()
{
    //iDDSServer.stop();
    std::cout << "Desctructor called" << std::endl;
}

OPENDAQ_DEFINE_CLASS_FACTORY_WITH_INTERFACE(
    INTERNAL_FACTORY, iDDSServer, daq::IServer,
    daq::DevicePtr, rootDevice,
    PropertyObjectPtr, config,
    const ContextPtr&, context
)

END_NAMESPACE_OPENDAQ_IDDS_SERVER_MODULE
