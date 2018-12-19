local Default_Components = require('default_components')
local Default_Actions = require('default_actions')

entity.intro_police = {}

entity.intro_police.Graphic_Component = {
   Animations = {
	  stand = {
		 "intro_police_stand.png",
	  },
   },
   default_animation = "stand",
   z = 1
}

entity.intro_police.Position_Component = {}

entity.intro_police.Collision_Component = {
   kind = Collider_Kind.DYNAMIC,
   bounding_box = Default_Components.Human_Bounding_Box,
   on_collide = function(this, other) end
}

entity.intro_police.Movement_Component = Default_Components.Human_Movement_Component

entity.intro_police.Interaction_Component = {
   on_interact = function(this, other) end
}

entity.intro_police.Action_Component = {
   update = function(this, dt) end
}

entity.intro_police.Task_Component = {}


-- Scripts
entity.intro_police.scripts = {}

entity.intro_police.scripts.intro = {
   Default_Actions.Teleport_Off_Screen,
}
   
entity.intro_police.scripts.intro1 = {
   {
	  kind = "Teleport_Action",
	  dest = {
		 x = .5,
		 y = .5
	  },
	  is_blocking = true
   },
   {
	  kind = "Movement_Action",
	  dest = {
		 x = .5,
		 y = .5
	  },
	  is_blocking = true
   },
   {
	  kind = "Dialogue_Action",
	  dialogue = {
		 text = "What are you doing here? Attendance of the execution is mandatory under penalty of prison time.",
		 terminal = false,
		 responses = {
			"My apologies officer, I was just getting on my way.",
		 },
		 children = {
			{
			   text = "That you better, son. I've got my eye on you.",
			   terminal = true,
			   responses = {},
			   children = {}
			}
		 }
	  }
   },
   {
	  kind = "Movement_Action",
	  dest = {
		 x = 0,
		 y = 0
	  },
	  is_blocking = true
   },
   Default_Actions.Teleport_Off_Screen
}


-- State machine
entity.intro_police.State_Machine = {}

entity.intro_police.State_Machine.intro = {
   task = entity.intro_police.scripts.intro,
   transitions = {
	  {
		 vars = {
			intro_door_answered = true,
		 },
		 next_state = "intro1"
	  }
   }
}

entity.intro_police.State_Machine.intro1 = {
   task = entity.intro_police.scripts.intro1,
   transitions = {}
}
