project('harpi_test', 'cpp')

inc = include_directories(
	[
		'main',
		'main/fake',
	])
test_led_src = [
	'main/test/Test.Led.cpp',
	'main/fake/Adafruit_NeoPixel.cpp',
	'main/Led.cpp',
]
test_buzzer_src = [
	'main/test/Test.Buzzer.cpp',
	'main/fake/esp32-hal-ledc.cpp',
	'main/Buzzer.cpp',
]
test_led = executable('Test.Led', test_led_src, include_directories : inc)
test('led', test_led)
test_buzzer = executable('Test.Buzzer', test_buzzer_src, include_directories : inc)
test('buzzer', test_buzzer)
