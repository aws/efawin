# Efawin - Efa library for Windows

Efawin library helps applications on Windows to interact with Elastic Fabric Adapter hardware on AWS EC2.
It provides the necessary functionality through an interface that is very close to 
infiniband verbs interface. At this time, this library is scoped to support the libfabric
efa provider that is part of the Open Fabrics Interfaces (OFI). See 
[the OFI web site](http://libfabric.org) for more information on libfabric.  Any other use of the library is not tested or supported.

## Library functionality

This library provides the necessary functionality to create and manipulate the queues associated with AWS Elastic Fabric Adapter. Majority of the library's functionality comes from rdmacore's [efa provider](https://github.com/linux-rdma/rdma-core/tree/master/providers/efa). 
To allow compilation of the library on Windows, some helper/compat files have been copied from the OFI's [libfabric](https://github.com/ofiwg/libfabric).
The Efa driver interaction files have been copied from a AWS fork of [libfabric](https://github.com/aws/libfabric/tree/v1.9.x-cdi).
In addition to these, the Infiniband header files from OpenIB are used to provide an interface to the library.

## Building and installing Efawin from source

Efawin can be built from a git clone or a zip file downloaded from github.
Efawin requires Microsoft Visual Studio 2019 or higher to be compiled. This can be installed from 
[Microsoft's website](https://visualstudio.microsoft.com/downloads/).

The solution file `efawin.sln` can be opened in the IDE and built using the `Build` menu option.  
You can also build it using the command line tool `msbuild` from a Visual Studio developer 
powershell/command prompt.

Once built, the generated efawin.dll must be placed in the same folder as libfabric.dll for the
efa provider in libfabric to load it during execution.

## Using Efawin in your project

To use Efawin in your project, copy the contents of interface folder into your project and use `efa_load_efawin_lib` function to load the dll and `efa_free_efawin_lib` function to free the dll.  The supported list of `ibv_*` functions can be found in `interface\efawinver.h`. `interface\infiniband\verbs.h` will provide the function declarations for your application.

## Runtime considerations

The version of efawin dll used must be compatible with the installed efa driver version.  Efawin dll will return a failure when it cannot interact with a compatible version of efa driver.
Efawin dll uses `EFA_API_INTERFACE_VERSION` from `efaioctl.h` when interacting with the driver.
If you have an incompatible driver, please update your efa driver to match the EFA_API_INTERFACE_VERSION as listed below.

Driver version compatibility is as listed below:
- EFA_API_INTERFACE_VERSION 1 is compatible with efa.sys version 1.0.0.4
- EFA_API_INTERFACE_VERSION 2 is compatible with efa.sys version 1.1.0.9

EFA driver for Windows can be installed using the AWS public release S3 bucket: https://ec2-windows-drivers-efa.s3-us-west-2.amazonaws.com/Latest/EFADriver.zip

## Validate installation

There is no separate test for efawin installation at this time.
The fi_info utility from libfabric can be used to validate efawin installation.  It will return the efa fabric attributes on successful installation.
See [libfabric validation](https://github.com/ofiwg/libfabric#validate-installation) for details.

## Security

See [CONTRIBUTING](CONTRIBUTING.md#security-issue-notifications) for more information.

## License

Unless otherwise stated in individual source, this software is available under one of two licenses you may choose from: [GPL-2.0](LICENSE-GPL-2.0) OR
[OpenIB.org BSD license](LICENSE-Linux-OpenIB)
