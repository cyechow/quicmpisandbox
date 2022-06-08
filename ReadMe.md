# MsQuic + MPI

This is a proof-of-concept sandbox for using [MsQuic](https://github.com/microsoft/msquic) with [MPI](https://github.com/Microsoft/Microsoft-MPI).

* MsQuic API Documentation: [link](https://github.com/microsoft/msquic/tree/main/docs/api)
* Borrowed code from [sample.c](https://github.com/microsoft/msquic/blob/main/src/tools/sample/sample.c) (see [License](#license))

This is meant for a desktop application that can also be run via command line.

## Build Notes
* Built successfully with vS2019. Should be able to restore nuget packages and build without problems.
* Only works in debug (see [TODO](#todo)).
* Requires MPI installed to test, installers [here](https://github.com/microsoft/Microsoft-MPI/releases/tag/v10.1.1).
* Run `mpiexec -n 2 QuicMPISandbox.exe` through command line to test on machine.
* In `main.cpp`'s `main` function:
  * Use `RunQuicOnlyTest()` to run without MPI involved, again only works in Debug.
* Added in .pem files for testing, these aren't used anywhere and is only for testing. Don't use in any production code.

## Notes

My goal is to pass all data to a single writer rank to write to a local SQLite database.

Reasons for exploring MsQuic instead of just using MPI to pass data between ranks:
* There are data limitations to message passing via MPI, I ran into a limit of 4mb per rank.
* The data per rank that needs to be passed to the writer will definitely exceed 4mb, minimum will be 5mb.
* The specs of the machine(s) running the application isn't under my control so this has to work regardless of hardware.

## TODO
* Figure out why we get QUIC_STATUS_TLS_ERROR (QuicDriver::ServerLoadConfiguration: ConfigurationLoadCredential failed, 0x80072b18!) in Release but not Debug.
* Figure out a better way to create testing SSL certificates other than adding to repository.
* Clean up artifacts (.dll & .pem files) on project/solution clean.
* Centralize the logging s.t. timestamp and rank # are added with each log.

## License
* Code from MsQuic is open sourced under the MIT license, see [here](https://github.com/microsoft/msquic/blob/main/LICENSE) for full license.
