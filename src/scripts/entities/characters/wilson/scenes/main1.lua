-- Script
entity.wilson.scripts.main1 = {
   {
	  kind = "Wait_For_Interaction_Action",
	  is_blocking = true
   },
   {
	  kind = "Dialogue_Action",
	  dialogue = {
		 text = "*main1 dialogue*",
		 terminal = true,
		 responses = {},
		 children = {}
	  }
   }
}

-- State machine
entity.wilson.State_Machine.main1 = {
   task = entity.wilson.scripts.main1,
   transitions = {}
}