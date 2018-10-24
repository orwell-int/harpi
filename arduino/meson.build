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
test_led = executable('Test.Led', test_led_src, include_directories : inc)
test('led', test_led)
