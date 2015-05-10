<?php
require_once(dirname(__FILE__) . '/SimClient.php');

$ip = '127.0.0.1';
$port = 8800;
$sim = new SimClient($ip, $port, -1);

$req = array(
	'echo',
	'你好',
	'hello world!',
	'[hi]',
	array(
		'a',
		'中',
		'国',
		'b',
	),
);
$sim->send($req);
$resp = $sim->recv();
var_dump($resp);

