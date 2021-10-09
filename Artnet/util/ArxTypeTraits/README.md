# ArxTypeTraits

C++ type_traits for Arduino which cannot use it as default


## Features

- automatically use standard library first if the boards can
- if standard library is not enough (e.g. only C++11 is available), add missing parts listed below
- works almost all Arduino platforms (Let me know if you have errors)
- compatible with [ArduinoSTL](https://github.com/mike-matera/ArduinoSTL) or other [uClibc++](https://www.uclibc.org/) libraries
  - thx @matthijskooijman


## Supported Class Templates

### C++11 (defined only for boards before C++11)

- `std::integral_constant`
- `std::true_type`
- `std::false_type`
- `std::declval`
- `std::enable_if`
- `std::conditional`
- `std::remove_cv`
- `std::remove_const`
- `std::remove_volatile`
- `std::remove_pointer`
- `std::remove_reference`
- `std::remove_extent`
- `std::add_pointer`
- `std::forward`
- `std::is_same`
- `std::is_integral`
- `std::is_floating_point`
- `std::is_arithmetic`
- `std::is_signed`
- `std::is_unsigned`
- `std::is_pointer`
- `std::is_array`
- `std::is_convertible`
- `std::is_function`
- `std::is_empty`
- `std::decay`
- `std::result_of`


#### for utility

- `std::numeric_limits` (only `max()` and `min()` now)
- `std::swap`
- `std::initializer_list`
- `std::tuple`
- `std::get`
- `std::tuple_size`
- `std::function`


### C++14 (defined only for boards before C++14)

- `std::enable_if_t`
- `std::decay_t`
- `std::remove_cv_t`
- `std::remove_const_t`
- `std::remove_volatile_t`
- `std::remove_reference_t`
- `std::remove_pointer_t`
- `std::integer_sequence`
- `std::index_sequence`
- `std::make_index_sequence`
- `std::index_sequence_for`


### C++17 (defined only for boards before C++17)

- `std::void_t`
- `std::disjunction`
- `std::conjunction`
- `std::negation`
- `std::apply`


### C++2a

- `std::remove_cvref`
- `std::remove_cvref_t`


### Others (defined for all boards)

- `arx::is_detected`
- `arx::is_callable`
- `arx::function_traits`


## Used Inside of

- [Packetizer](https://github.com/hideakitai/Packetizer)
- [MsgPack](https://github.com/hideakitai/MsgPack)
- [MsgPacketizer](https://github.com/hideakitai/MsgPacketizer)
- [ArduinoOSC](https://github.com/hideakitai/ArduinoOSC)
- [ArtNet](https://github.com/hideakitai/ArtNet)
- [PollingTimer](https://github.com/hideakitai/PollingTimer)
- [Tween](https://github.com/hideakitai/Tween)
- [ArxStringUtils](https://github.com/hideakitai/ArxStringUtils)
- [Filters](https://github.com/hideakitai/Filters)
- [Debouncer](https://github.com/hideakitai/Debouncer)


## Contributors

- [Matthijs Kooijman](https://github.com/matthijskooijman)


## License

MIT
