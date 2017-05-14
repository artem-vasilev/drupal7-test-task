<?php
/**
 * @file
 *
 * Contains batch callbacks
 */

/**
 * @param integer $userId optionally can be integer[]
 * @param string $lastName
 * @param integer $usersAmount
 */
function batch_user_update_process($userId, $lastName, &$context) {
  $userIds = is_array($userId) ? $userId : [$userId];

  if (empty($context['sandbox'])) {
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['max'] = count($userIds);
  }

  $users = entity_load('user', $userIds);

  foreach ($users as $user) {
    $user->last_name[LANGUAGE_NONE][0]['value'] = $lastName;

    field_attach_presave('user', $user);
    field_attach_update('user', $user);

    $context['sandbox']['progress']++;
    $context['results'][] = "$user->name (uid: $user->uid)";
  }

  $context['finished'] = $context['sandbox']['progress'] >= count($userIds);
}

/**
 * @param $success
 * @param $results
 * @param $operations
 */
function batch_users_update_finished($success, $results, $operations) {

  if ($success) {
    $message = t('@count users successfully updated:', ['@count' => count($results)]);
    $message .= theme('item_list', ['items' => $results]);
    drupal_set_message($message);
  } else {
    $error_operation = reset($operations);

    $message = t(
      'An error occurred while processing %error_operation with arguments: @arguments',
      [
        '%error_operation' => $error_operation[0],
        '@arguments' => print_r($error_operation[1], TRUE)
      ]
    );

    drupal_set_message($message, 'error');
  }
}