# Json Serialization Plugin
This plugin adds support for serializing UObjects to json. It supports nested objects, arrays, structs, etc. It serializes all non-transient properties, and it does not serialize cyclical references.

## How to use

It comes with one function:

`TSharedPtr<FJsonObject> FJsonSerializationModule::SerializeUObjectToJson(const UObject* Object)`

It's also got a blueprint function that does the same thing:

![Blueprint Object to Json function](Resources/ObjectToJson.PNG?raw=true")

If you use the blueprint function, remember to enable the `Json Blueprint Utilities` plugin to be able to convert the outputted json to a string in BP.

## Example output

Here's an example of the output. Here's a blueprint:

![Example blueprint](Resources/analytics_collector_object.png?raw=true")

And it's json serialization:

```json
{
    "StartTime": "2023.06.29-19.28.24",
    "EndTime": "2023.06.29-19.28.25",
    "BuildData": {
        "BuildVersion": "++UE5+Release-5.2-CL-25360045",
        "BuildConfiguration": "Development",
        "PlatformName": "Windows"
    },
    "PlayerControllerData": [
        {
            "IsLocal": true,
            "PawnData": [
                {
                    "ClassName": "BP_PlayerCactusCharacter_C",
                    "PossessTime": "2023.06.29-19.28.24",
                    "Level": "/Game/Cactus/Maps/JobInterview/JobInterview.JobInterview:PersistentLevel",
                    "ExtraData": {
                        "DeathData": []
                    }
                }
            ]
        }
    ],
    "IsUsingSteam": false,
    "IsPlayInEditorSession": true
}
```
As you can see, it supports nested objects (`BP_PlayerControllerData`) and arrays and structs.
