# TODO

* add json path support to from_json so that one can only parse submembers
* add escape support or validate it with json path
* support inf/nan in floating point numbers
* implement arrow proxy on json array iterator
* add fallback to iostreams to string encoding/decoding
* allow member_path to allow parsing array indexes
* tag disriminator for variant classes.  Merge the member's into a special merged type that allows a callable to select which variant item to construct
