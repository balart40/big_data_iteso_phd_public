Author: M.S. Francisco Eduardo Balart Sanchez <balart40@hotmail.com> <ng711690@iteso.mx>

Collaborator(s): Dr. Luis Fernando Preciado

To run with visualizer put the following files in your YOURPATH/ns/ns-allinone-3.25/ns-3.25/scratch/ directory
note that for this example we are using ns3.25
Then go to the YOURPATH/ns/ns-allinone-3.25/ns-3.25/
* To run without visualizer and save the output NS LOG messages in a log file use:

   sudo ./waf --run scratch/manet_TxRx 2>logfile
* To run with visualizer use: 

   sudo ./waf --run scratch/manet_TxRx --vis
* The run will leave a .xml file, this can be used if you have the net anim API
    * Just go to your YOURPATH/ns/ns-allinone-3.25/ns-3.25 directory and type
    
    ../netanim-3.107/NetAnim &
* Load the xml file click the play button and enjoy! :)
