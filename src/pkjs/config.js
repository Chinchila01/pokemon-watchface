module.exports = [
	{
		"type": "heading",
		"defaultValue": "Configure"
	},
	{
		"type": "section",
		"items": [
			{
				"type": "heading",
				"defaultValue": "Pokemon"
			},
			{
				"type": "radiogroup",
				"messageKey": "pokemon",
				"defaultValue": "0",
				"label": "Pokemon",
				"options": [
					{
						"label": "Grookey",
						"value": "0"
					},
					{
						"label": "Scorbunny",
						"value": "1"

					},
					{
						"label": "Sobble",
						"value": "2"
					}
				]
			}
		]
	},
	{
		"type": "section",
		"items": [
			{
				"type": "heading",
				"defaultValue": "Timed Cycle"
			},
			{
  				"type": "toggle",
  				"messageKey": "cycle",
  				"label": "Cycle",
  				"defaultValue": false
			},
			{
			  "type": "slider",
			  "messageKey": "minutes_value",
			  "defaultValue": 1,
			  "min": 1,
			  "max": 60,
			  "step": 1
			},
			{
			  "type": "slider",
			  "messageKey": "hours_value",
			  "defaultValue": 1,
			  "min": 1,
			  "max": 24,
			  "step": 1
			},
			{
			  "type": "select",
			  "messageKey": "time_unit",
			  "defaultValue": "0",
			  "options": [
			    { 
			      "label": "hours", 
			      "value": "0" 
			    },
			    { 
			      "label": "minutes",
			      "value": "1" 
			    },
			  ]
			}
		]
	},
	{
    	"type": "submit",
    	"defaultValue": "Save Settings"
	}
]
