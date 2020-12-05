--TEST--
Test TypeMapper basic serialization
--FILE--
<?php
foreach(
[
	[ 'message' => 'Hello!' ],
	[ 'message' => 'Hello!', 'type' => 'greeting' ],
	true,
	false,
	5,
	0,
	-1,
	-999999999,
	'Hello!',
] as $a)
{
	$m = new \VPack\TypeMapper();
	echo implode(" ", str_split(bin2hex($m->serialize($a)), 2))."\n";
}
?>
--EXPECT--
14 12 47 6d 65 73 73 61 67 65 46 48 65 6c 6c 6f 21 01
0b 22 02 47 6d 65 73 73 61 67 65 46 48 65 6c 6c 6f 21 44 74 79 70 65 48 67 72 65 65 74 69 6e 67 03 12
1a
19
35
30
2f ff ff ff ff ff ff ff ff
2f 01 36 65 c4 ff ff ff ff
46 48 65 6c 6c 6f 21