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
    	"type": "submit",
    	"defaultValue": "Save Settings"
	}
]
