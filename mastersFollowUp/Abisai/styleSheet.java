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
