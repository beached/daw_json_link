# Deduction concepts

These files are used in constraints, errors, and type deduction.  They provide a way of telling the system that some types are able to fullfill these concepts, via specialization.

## Current Concepts

  * Writable Output Types - `writable_output_traits` - A type that can be serialized via write and put. 
  * Readable Value - `nullable_value_traits` - Option/Nullable/Readable like types that can be constructed, tested for whether they container a value, and read from.