# TODO

* add json path support to from_json so that one can only parse submembers
* add escape support or validate it with json path
* support inf/nan in floating point numbers
* change Signed/Unsigned parser tree to keep intmax_t/uintmax_t as parse_to_t until casting to type at end
* implement arrow proxy on json array iterator
* swap JsonElement and Container template arguments to json_array so that we can default to a vector of elements parse_to_t
