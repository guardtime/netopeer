# Testing Orca

Testing the Orca system involves using a [client](../cli) to send XML messages to the modified [Netopeer server](../server). The server responses must then be checked for errors.

A number of preparared XML messages can be found in [messages](./messages). A [shell script](./run_tests.sh) automates the test process by sending each XML message to the server, capturing the responses and compiling a test report.

## Automated testing
To run the automated tests **make sure you have setup a user account so that it can connect to the netconf server using public key authentication**. See [SSH Public Key Authentication](https://guardtime.atlassian.net/wiki/display/OE/SSH+public+key+authentication) for more information.

Enter the test directory and type 'make'.

~~~
cd test
make
make clean  # CAUTION! This will delete the test report and the logs
~~~

The response of each message is stored as a ".out" file in the *results/* directory. The generated test report is saved as *results/test_report*

If more complex test scenarios are needed a separate shell script based on [run_tests.sh](./run_tests.sh) can be written for each test case. The individual test scripts can then be run by the top-level script *run_tests.sh*.

## Manual testing
Manual testing can be done either by running *netopeer-cli* and issuing commands at its prompt

~~~
netopeer-cli
~~~

Or by passing arguments from the shell command line as follows:

~~~
netopeer-cli <<ORCATEST
connect --login myuser
user-rpc --file path/to/message.xml
disconnect
ORCATEST
~~~

