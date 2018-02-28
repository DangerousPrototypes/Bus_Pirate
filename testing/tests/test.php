<?php
/**
 * Created by PhpStorm.
 * User: ian
 * Date: 2/26/2018
 * Time: 4:41 PM
 */

$test = [
    'hardware' => ['3', '4'],//hardware versions to test
    #'firmware' => ['<=8'],//firmware versions to test
    'device' => '24LC04B',//name of this test
    'position' => null,//where on the test rig chain, null for no test rig
    'test' => 
	[
        0 => 
		[
			'name' =>'setup',
			'steps'=>
			[
                [
                    'hardware' => ['3', '4', '5'],//this command, versions applies to
                    'linefeed' => true,//with <enter> key after each command?
                    'commands' => [
                        'm',//mode
                        '4', //I2C
                        '1',//speed select
                    ]
                ],
                [
                    'hardware' => ['3'],//this command, versions applies to
                    'linefeed' => true,//with <enter> key after each command?
                    'commands' => [
                        'P',//pullups on
                        'W', //power on
                    ]
                ],
                [
                    'hardware' => ['4', '5'],//this command, versions applies to
                    'linefeed' => true,//with <enter> key after each command?
                    'commands' => [
                        'P',//pullups on
                        '3',//3.3volt pullups
                        'W', //power on
                    ]
                ]

            ]
        ],
        1 => 
		[
			'name'=>'write',
            'steps'=>
			[
                [
                    'hardware' => ['3', '4', '5'],//this command, versions applies to
                    'linefeed' => true,//with <enter> key after each command?
                    'commands' => [
                        '[0xa0 0 0 1 2 3 4 5 6 7 8]',//write data to eeprom
                    ]
                ]

            ]
        ],
        2 => 
		[
			'name'=>'read',
            'steps'=>
			[
				[
					'hardware' => ['3', '4', '5'],
					'linefeed' => true,
					'commands' => [
						'[0xa0 0 [ 0xa1 r:9]', //read back from eeprom
					]
				]
			]
        ]
    ]
];

echo json_encode($test,JSON_PRETTY_PRINT);