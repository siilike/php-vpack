<?php
$input = file_get_contents('test.vpack');

$vp = new \VPack\Slice($input);

var_dump($vp->typeName());
var_dump($vp->toJson());
