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
