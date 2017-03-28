package distributedNodes;

import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Collections;

import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.concurrent.ConcurrentHashMap;

import org.graphstream.graph.*;
import org.graphstream.graph.implementations.*;
import org.graphstream.stream.file.FileSinkDGS;
import org.graphstream.ui.graphicGraph.stylesheet.StyleConstants.Units;
import org.graphstream.ui.spriteManager.Sprite;
import org.graphstream.ui.spriteManager.SpriteManager;

public class SimulationEESOA {
	// this is taken from the graphstream library
	public Graph graph;
	
	private int iteration =0;
	public int maxNodes = 20; //max number of nodes in the graph
	public int numAttempts = 70; //number of attempts to create a new node or edge (in the case that the node exists)
	public double probability = 1.0;  // probability to create a new node or edge (in the case that the node exists)
	
	enum solutionType{ESSOA, DOT};
	solutionType solucion =solutionType.ESSOA;
	
	// SolutionESSOA has a HashMap with key as the id of the node ( 1,2,3..etc) and the value is its
	// Corresponding NodeESSOA object
	public ConcurrentHashMap<Integer, NodeESSOA> solutionESSOA = new ConcurrentHashMap<Integer,NodeESSOA>();
	public Tests tests;
		
	public static void main(String[] args)
	{
		System.setProperty("org.graphstream.ui.renderer", "org.graphstream.ui.j2dviewer.J2DGraphRenderer");
		new SimulationEESOA().simulation();  	
	}
	
	/***************************************************************
	 *    SIMULATION BEGINS HERE
	 ***************************************************************/
	public void simulation(){
		System.out.println("********BEGIN OF SIMULATION***********");
		
		// Creation of the graph first through graphstream
		graph = new SingleGraph("tutorial 1");
		graph.setAutoCreate(true);
        graph.setStrict(false);
		graph.display();
        graph.addAttribute("ui.stylesheet", styleSheet);
        // Create sprites for nodes label identification by febalart
        SpriteManager sman = new SpriteManager(graph);
        Sprite s1 = sman.addSprite("Leader");
        Sprite s2 = sman.addSprite("Gateway");
        Sprite s3 = sman.addSprite("Member");
        Sprite s4 = sman.addSprite("Bridge");
        s1.setPosition( Units.PX, 40, 500, 0);
        s1.addAttribute("ui.label", "          Leader");
        s2.setPosition( Units.PX, 40, 520, 0);
        s2.addAttribute("ui.label", "          Gateway");
        s3.setPosition( Units.PX, 40, 540, 0);
        s3.addAttribute("ui.label", "          Member");
        s4.setPosition( Units.PX, 40, 560, 0);
        s4.addAttribute("ui.label", "          Bridge");
        // added by febalart
        LinkedList<Node> BFSGraph = new LinkedList<Node>();
        LinkedList<Node> BFSESSOAGraph = new LinkedList<Node>();
        LinkedList<Integer> GraphNodes = new LinkedList<Integer>();
        LinkedList<Integer> GraphNodesBFS = new  LinkedList<Integer>();
        LinkedList<Integer> ESSOANodesBFS = new  LinkedList<Integer>();
        
        // Begin "REAL" graph and simulation starting from the 
        // Graph stream graph nodes created
        tests = new Tests(graph); //associates the graph to the tests
        
        //*********!!!! PUT YOUR TESTS HERE!!! ************
        //  YOUR BASE BELONG TO US
        //  TAKE ME TO YOUR LEADER
        //  WHY NOT SZOYDBERG 
        //  (V) (;,,,;)  (V)
        
        //for(int i = 0; i < numAttempts; i++)
        //	tests.testTotallyDynamic(maxNodes,probability);
        //tests.TestBasicSegmentation_sameId_two_segments(0);
        //tests.TestTwoNetworksBridges(0);
        tests.OneNetworksBridgesandGateways(0);
        // tests.TestTwoNetworks(0);
        //tests.TestBasicSegmentation_sameId(0);
        //tests.TestBasicSegmentation(0);
        //tests.testbridges2(0);
        //tests.testbridges2(0); 
        //tests.test5Dynamic(0);
        
        //******************************************************
        
        // from nodes created on graphstream we will create our NodeESSOAs
        for(Node n:graph)
        {
        	NodeESSOA nbb = new NodeESSOA(Integer.parseInt(n.getId()),n);
        	solutionESSOA.putIfAbsent(Integer.parseInt(n.getId()), nbb);
        	n.setAttribute("ui.label",n.getId());
        	// added by febalart
        	GraphNodes.add(Integer.parseInt(n.getId()));
        	BFSGraph.add(n);
  	    }
        
        LinkedList<LinkedList<Integer>> NetworkGraphsBFS = new LinkedList<LinkedList<Integer>>();
        // BFS Before ESSOA
        while(!BFSGraph.isEmpty())
        {	
        	GraphNodesBFS = new LinkedList<Integer>();
        	GraphNodesBFS = GraphBfs(BFSGraph, false);
        	NetworkGraphsBFS.add(GraphNodesBFS);
        }
        System.out.println("Vertexes of Whole Graph Network/Segments after BFS");
        System.out.println(NetworkGraphsBFS+"\n");
             
        // Save Graph before ESSOA executes
        SaveGraph("Pre");  
        //perform the iterations
        Integer rVal[]={-1};
        // 500 iterations
    	for(iteration =0; iteration <500; iteration++)
    	{
    	    	performNodeTask(solutionType.ESSOA);
    	       	sleep(50);
    	    	if(iteration == 300)
    	    	{
    	    		tests.testDynamicRemove(maxNodes, 1.0,rVal);
    	       	}
    	}
    	
    	for(Node n: graph)
    	{
    		BFSESSOAGraph.add(n);
    	}
    	
    	LinkedList<LinkedList<Integer>> NetworkESSOABFS = new LinkedList<LinkedList<Integer>>();
        while(!BFSESSOAGraph.isEmpty())
        {	
            ConcurrentHashMap<Integer, ? extends NodeAbstract> nodes = solutionESSOA;
    		nodes = solutionESSOA;
    		// ensure first node to explore will not be a member
    		if(nodes.get(Integer.parseInt(BFSESSOAGraph.get(0).getId())).nodeInfo.role.equals("member"))
    		{
    			BFSESSOAGraph.remove(0);
    			continue;
    		}
    		//System.out.println("\nFirst node to explore is "+nodes.get(Integer.parseInt(BFSESSOAGraph.get(0).getId())).nodeInfo.role+" id: "+Integer.parseInt(BFSESSOAGraph.get(0).getId())+"\n");
        	ESSOANodesBFS = new LinkedList<Integer>();
        	ESSOANodesBFS = GraphBfs(BFSESSOAGraph, true);
        	NetworkESSOABFS.add(ESSOANodesBFS);
        }
    	
    	
    	// Save Graph After ESSOA executes
    	SaveGraph("Post");
    	System.out.println("\nNodes of original Graph");
    	System.out.println(GraphNodes);
    	System.out.println("\nNodes of ESSOA graph");
    	System.out.print(NetworkESSOABFS);
    	System.out.println("\n********END OF SIMULATION***********");
	}
	
	public LinkedList<Integer> GraphBfs(LinkedList<Node> BFSGraph, boolean isESSOA)
	{
       
		LinkedList<Integer> QueueBFS = new LinkedList<Integer>();
        LinkedList<Integer> BFSVisited = new LinkedList<Integer>();
        LinkedList<Integer> GraphNodesBFS = new LinkedList<Integer>();
        Node nextVrtx = null;
        Node CurrentVrtx = null;
        Node InitialVrtx;
	    boolean isBackBone;
	    
        // BFS of graph of Graphstream
        InitialVrtx = BFSGraph.get(0);
        // Initialize for each network/segment found
        BFSVisited = new LinkedList<Integer>();
        QueueBFS = new LinkedList<Integer>();
    	
        BFSVisited.add(Integer.parseInt(InitialVrtx.getId()));
        QueueBFS.add(Integer.parseInt(InitialVrtx.getId()));
        GraphNodesBFS.add(Integer.parseInt(InitialVrtx.getId()));
        
        while(!QueueBFS.isEmpty())
        {	
        	for(Node n : BFSGraph)
        	{
        		if(Integer.parseInt(n.getId()) == QueueBFS.get(0))
        		{
        			CurrentVrtx = n;
        		}
        	}	      
        	QueueBFS.remove(0);
        	BFSGraph.remove(CurrentVrtx);
        	Iterator<? extends Node> kit = CurrentVrtx.getNeighborNodeIterator();
        	while(kit.hasNext())
        	{
        		// get next vertex regardless if is essoa or nor        		
        		nextVrtx = kit.next();
        		if(isESSOA)
        		{
        			//System.out.println("Current vrtx " + solutionESSOA.get(Integer.parseInt(CurrentVrtx.getId())).nodeInfo.role+" id: "+Integer.parseInt(CurrentVrtx.getId()));
            		//System.out.println("Next vrtx " + solutionESSOA.get(Integer.parseInt(nextVrtx.getId())).nodeInfo.role+" id: "+Integer.parseInt(nextVrtx.getId()));
        			isBackBone = checkESSOAbackbone(CurrentVrtx, nextVrtx);
        			if(!isBackBone)
        			{
        				// remove neighbor
        				//System.out.println("removing neighbor "+solutionESSOA.get(Integer.parseInt(nextVrtx.getId())).nodeInfo.role+" id "+Integer.parseInt(nextVrtx.getId())+"\n");
            			BFSVisited.add(Integer.parseInt(nextVrtx.getId()));
        			}
        		}
        		if(!BFSVisited.contains(Integer.parseInt(nextVrtx.getId())))
        		{
        			BFSVisited.add(Integer.parseInt(nextVrtx.getId()));
        			QueueBFS.addFirst(Integer.parseInt(nextVrtx.getId()));
        			GraphNodesBFS.add(Integer.parseInt(nextVrtx.getId()));
        		}
        	}
        }
        //System.out.println("returning essoa graph: "+GraphNodesBFS+"\n");
        return GraphNodesBFS;
	}
	
	public boolean checkESSOAbackbone(Node CurrentVrtx, Node NextVrtx)
	{
		boolean isbackbone = false;
		Node CurrentVrtxTocheck = CurrentVrtx;
		Node CurrentVrtxTocheckNeigh = NextVrtx;
		ConcurrentHashMap<Integer, ? extends NodeAbstract> nodes = solutionESSOA;
		nodes = solutionESSOA;

		if( ( (nodes.get(Integer.parseInt(CurrentVrtxTocheck.getId())).nodeInfo.role.equals("leader")) || (nodes.get(Integer.parseInt(CurrentVrtxTocheck.getId())).nodeInfo.role.equals("gateway")) || (nodes.get(Integer.parseInt(CurrentVrtxTocheck.getId())).nodeInfo.role.equals("bridge"))) 
				&& (nodes.get(Integer.parseInt(CurrentVrtxTocheckNeigh.getId())).nodeInfo.role.equals("member")))
		{
			isbackbone = false;
		}
		else
		{
			isbackbone = true;
		}
		return isbackbone;
	}
	
	
	  public void performNodeTask(solutionType solution){
		
		  ConcurrentHashMap<Integer, ? extends NodeAbstract> nodes = solutionESSOA;
		  Graph activeGraph = graph;
		  nodes = solutionESSOA;
		  activeGraph = graph;

		  createNeighborTableESSOA();

		  int numLeaders = 0;
		  int numGateways = 0;
		  int numMembers = 0;
		  int numBridges = 0;

		  List<Node> listNodes = new ArrayList<Node>();
		  listNodes.addAll(activeGraph.getNodeSet());
		  Collections.shuffle(listNodes); //array list randomly disordered 

		  for(Node n:listNodes)
		  {
			  int id = Integer.parseInt(n.getId());
			  // Here is where we perform the ESSOA algorithm per node
			  nodes.get(id).updateQuality();
			  
			  if( nodes.get(id).nodeInfo.role.equals("leader"))
				  numLeaders++;
			  else if(nodes.get(id).nodeInfo.role.equals("gateway"))
				  numGateways++;
			  else if (nodes.get(id).nodeInfo.role.equals("bridge"))
				  numBridges++;
			  else if (nodes.get(id).nodeInfo.role.equals("member"))
				  numMembers++;
		  }
		  //System.out.println("\n");
		  if(iteration % 100 == 0)
			  System.out.println("IT: "+iteration+" "+solution.toString() +": L = "+numLeaders+", G = "+ numGateways+", M = "+numMembers+", B="+numBridges );


		  for(Edge edge: activeGraph.getEachEdge()) {
			  edge.addAttribute("ui.class", "standard");
			  Node n0 =  edge.getNode0();
			  Node n1 =  edge.getNode1();

			  if(nodes.get(Integer.parseInt(n0.getId())).nodeInfo.role.equals("leader") ||
					  nodes.get(Integer.parseInt(n1.getId())).nodeInfo.role.equals("leader")	|| 
					  (nodes.get(Integer.parseInt(n0.getId())).nodeInfo.role.equals("bridge") &&
							  nodes.get(Integer.parseInt(n1.getId())).nodeInfo.role.equals("bridge"))){
				  edge.addAttribute("ui.class", "backbone");
			  }

			  if((nodes.get(Integer.parseInt(n0.getId())).nodeInfo.role.equals("member") &&
					  nodes.get(Integer.parseInt(n1.getId())).nodeInfo.role.equals("leader")) ||
					  (nodes.get(Integer.parseInt(n0.getId())).nodeInfo.role.equals("leader") &&
							  nodes.get(Integer.parseInt(n1.getId())).nodeInfo.role.equals("member"))){
				  edge.addAttribute("ui.class", "member");
			  }

		  }
	  }
	  
		public void createNeighborTableESSOA(){
			 ConcurrentHashMap<Integer, ? extends NodeAbstract> nodes = solutionESSOA;
			 Graph activeGraph = graph;
			for(Node n:activeGraph){
				
				//obtain the id of a node
				 int id = Integer.parseInt(n.getId());
				
				 //if a node with the corresponding id does not exist then create and initialize
				 if(!nodes.containsKey(id)){
					 NodeESSOA nbb = new NodeESSOA(id,n);
			  		solutionESSOA.put(id, nbb);
				 }
				 
				 //Create the table of neighbors
				 Iterator<? extends Node> k = n.getNeighborNodeIterator();
		         while (k.hasNext()) {
		        	 Node next = k.next();
		        	 int idn = Integer.parseInt(next.getId());
		        	 if(!nodes.containsKey(idn)){
						 NodeESSOA nbb = new NodeESSOA(idn,next);
						 solutionESSOA.put(idn, nbb);
					 }
		        	 solutionESSOA.get(id).addNeighbor(idn, nodes.get(idn));
		         }
			 }
		}
		

	  /**
	   * Delay
	   * @param val time to wait
	   */
	  
	  protected void sleep(int val) {
		  try { Thread.sleep(val); } catch (Exception e) {}
		  //System.out.print("Iteracion "+ iteration+" ");
		  try{
			  Runtime.getRuntime().exec("cls");
		  }catch(Exception e){}
	  }

	  
	  /**
	   * Saves the graph in a file Test_<date>_<posfix>.dgs
	   * @param posfix   it is useful to indicate if is before of after computing the backbone
	   */
	  public void SaveGraph(String posfix)
	  { 
	        Calendar cal = Calendar.getInstance();
	        SimpleDateFormat sdf = new SimpleDateFormat("dd_MM_yyyy__HH_mm_ss_SS");
	        String strDate = sdf.format(cal.getTime());
	        // from http://stackoverflow.com/questions/3481828/how-to-split-a-string-in-java
	        String [] strDateManipParts = strDate.split("__");
	        String strDateManipParts_date = strDateManipParts[0];
	        String strDateManipParts_time = strDateManipParts[1];
	        
	        try {
				graph.write(new FileSinkDGS(),"Test_date_"+strDateManipParts_date+"_time_"+strDateManipParts_time+"_"+posfix+".dgs");
			} catch (IOException e) {
				// TODO Auto-generated catch block
				System.out.println("Error al guardar");
				e.printStackTrace();
			}	     
	  }
	  
	  protected String styleSheet =
			  "edge.standard {" +
					  "	size: 1px;"+
					  "	fill-color: gray;"+
					  "	stroke-width: 1px;"+
					  "	stroke-mode: none;"+
					  "}" +
					  "edge.backbone {" +
					  "	size: 2px;"+
					  "	fill-color: red;"+
					  "	stroke-mode: none;"+
					  // "	shape:line;"+
					  "	shape:blob;"+
					  "}"+
					  "edge.member {" +
					  "	size: 2px;"+
					  "	fill-color: gray;"+
					  "	stroke-mode: none;"+
					  //"	shape:line;"+
					  "	shape:blob;"+
					  "}"
					  +
					  "node.member {" +
					  "	size: 15px, 15px;"+
					  "	shape: cross;"+
					  "   fill-color: gray;" +
					  "	stroke-color:black;"+
					  "	stroke-mode: plain;"+
					  "	stroke-width: 1px;"+
					  //"	text-background-mode:plain;"+
					  "	text-alignment:under;"+
					  "	text-color: blue;"+
					  "}" +
					  "node.leader {" +
					  "	size: 20px, 20px;"+
					  "	shape: circle;"+
					  "   fill-color: red, black;" +
					  "	fill-mode:gradient-radial;"+	
					  "	stroke-mode: plain;"+
					  // "	text-background-mode:plain;"+
					  "	text-alignment:under;"+
					  "	text-color: blue;"+
					  "}"
					  +
					  "node.bridge {" +
					  "	size: 15px, 15px;"+
					  "	shape: box;"+
					  "   fill-color: green;" +
					  "	stroke-mode: plain;"+
					  //"	text-background-mode:plain;"+
					  "	text-alignment:under;"+
					  "	text-color: blue;"+
					  "}"
					  +
					  "node.gateway {" +
					  "	size: 15px, 15px;"+
					  "	shape: box;"+
					  " fill-color: black;" +
					  "	stroke-mode: plain;"+
					  //"	text-background-mode:plain;"+
					  "	text-alignment:under;"+
					  "	text-color: blue;"+
					  "}"
					  +
					  "sprite#Leader {"+
					  "	size: 20px, 20px;"+
					  "	shape: circle;"+
					  " fill-color: red, black;" +
					  "	fill-mode:gradient-radial;"+
					  "	size: 15px;"+
					  "	stroke-mode: plain;"+
					  	"}"
					  +
			  		  "sprite#Gateway {"+
					  "	size: 15px, 15px;"+
					  "	shape: box;"+
					  " fill-color: black;" +
					  "	size: 15px;"+
					  "	stroke-mode: plain;"+
					  "	stroke-width: 1px;"+
					  	"}"
					  +
			  		  "sprite#Bridge {"+
					  "	size: 15px, 15px;"+
					  "	shape: box;"+
					  " fill-color: green;" +
					  "	size: 15px;"+
					  "	stroke-mode: plain;"+
					  "	stroke-width: 1px;"+
					  	"}"
					  +
			  		  "sprite#Member {"+
					  "	size: 15px, 15px;"+
					  "	shape: cross;"+
					  " fill-color: gray;" +
					  "	size: 15px;"+
					  "	stroke-mode: plain;"+
					  "	stroke-width: 1px;"+
					  	"}";
	  /* +
		        "node.removed {" +
	    		"	size: 30px, 30px;"+
		        //"	text-background-mode:plain;"+
		        //"	text-color: red;"+
		         "}";*/
}
