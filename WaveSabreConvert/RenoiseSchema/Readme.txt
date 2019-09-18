This Renoise schema has been modified from the original that comes with the program. 

Mods by Hoffman / Logicoma

- 2017-05-16

Added in some track mixer devices from previous schema RenoiseSong37.xsd as these changed in newer versions to a standard TrackMixerDevice.
Added PluginProperties from RenoiseSong37.xsd, another type which was seamingly renamed between versions.

- 2017-05-05

The original when deserialized loses the indexed order, which is vital when trying to reference some things. I've simply wrapped these collections in <xs:choice> so when deserialized come out as an array of generic objects but in the right order.

