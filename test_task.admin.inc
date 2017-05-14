<?php

function test_task_user_settings() {

  $form = [];

  $form['user_batch_size'] = [
    '#type' => 'select',
    '#title' => t('Batch size'),
    '#options' => [
      1 => 1,
      5 => 5,
      10 => 10,
      'all' => 'all'
    ],
    '#default_value' => 1,
    '#description' => t('Set how many users can be updated in one iteration'),
  ];

  $form['user_last_name'] = [
    '#type' => 'textfield',
    '#default_value' => '',
    '#required' => TRUE,
    '#description' => t('Enter <em>Last name</em> to be applied to  <b>all users!</b>'),
  ];

  $form['submit'] = [
    '#type' => 'submit',
    '#value' => t('Update')
  ];

  return $form;
}

/**
 * Implements hook_form_submit().
 */
function test_task_user_settings_submit($form, &$form_state) {

  $query = new EntityFieldQuery();
  $result = $query
    ->entityCondition('entity_type', 'user')
    ->entityCondition('entity_id', 0, '>')
    ->execute();

  $lastName = $form_state['values']['user_last_name'];

  if (!$result || empty($result['user'])) {
    return;
  }

  $users = array_keys($result['user']);
  $usersAmount = count($users);

  $operations = [];

  $batchUsers = $form_state['values']['user_batch_size'] !== 'all'
    ? array_chunk($users, $form_state['values']['user_batch_size'])
    : [$users];

  foreach ($batchUsers as $userId) {
    $operations[] = [
      'batch_user_update_process',
      [$userId, $lastName]
    ];
  }

  $batch = [
    'operations' => $operations,
    'finished' => 'batch_users_update_finished',
    'title' => t('Processing users'),
    'progress_message' => t('Processed @current out of @total.'),
    'file' => drupal_get_path('module', 'test_task') . '/user.batch.inc',
  ];

  batch_set($batch);
}

function test_task_request_settings() {
  $form = [];

  $form['secretapi'] = [
    '#type' => 'textfield',
    '#default_value' => '',
    '#required' => TRUE,

    '#description' => t('Enter secret api data for request'),
  ];

  $form['data1'] = [
    '#type' => 'textfield',
    '#default_value' => '',
    '#required' => TRUE,
    '#description' => t('Enter data1 argument for request'),
  ];

  $form['data2'] = [
    '#type' => 'textfield',
    '#default_value' => '',
    '#required' => TRUE,
    '#description' => t('Enter data2 argument for request'),
  ];

  $form['submit'] = [
    '#type' => 'submit',
    '#value' => t('Send')
  ];

  return $form;
}

/**
 * Implements hook_form_submit().
 *
 * Instead all of this we actually can use
 * $form['#method'] = 'get';
 * $form['#action'] = url('http://example.com');
 * but in this case we will have other arguments in the request
 * and can`t handle response
 */
function test_task_request_settings_submit($form, &$form_state) {
  $url = 'http://example.com';
  $result = drupal_http_request(
    $url,
    [
      'secretapi' => $form_state['values']['secretapi'],
      'data1' => $form_state['values']['data1'],
      'data2' => $form_state['values']['data2']
    ]
  );
  if ($result->code == 200) {
    $message = t('Request to !url was successful', ['!url' => $url]);
    drupal_set_message($message);
  } else {
    $message = t(
      'Request to example.com failed with code: @code, error: @error',
      [
        '@code' => $result->code,
        '@error' => $result->error
      ]
    );
    drupal_set_message($message, 'error');
  }
}