<?php

/**
 * @todo move contact options to separate contact.inc
 */

/**
 * Implementation of hook_user_import_form_field_match().
 */
function user_user_import_form_field_match() {
  $options                     = array();
  $options['user']['email']    = t('Email Address*');
  $options['user']['password'] = t('Password');
  $options['user']['roles']    = t('Roles');
  $options['user']['created']    = t('Account Creation Date');

  // $options['user']['username'] = t('Username');
  // $options['user']['uid'] = t('UID');
  // $options['user']['modified'] = t('Modified');
  // $options['user']['picture_uri'] = t('Picture');
  // $options['user']['timezone'] = t('Timezone');
  // $options['user']['md5_password'] = t('Password MD5');

  return $options;
}

/**
 * Implementation of hook_user_import_form_fieldsets().
 */
function user_user_import_form_fieldset($import, $collapsed) {

  $form = array();
  user_user_import_edit_roles_fields($form, $import, $collapsed);
  user_user_import_edit_email_fields($form, $import, $collapsed);

  return $form;
}

/**
 * Implementation of hook_user_import_form_update_user().
 */
function user_user_import_form_update_user() {
  $form['roles']    = array(
    'title' => t('Roles'),
    'description' => t('Affected: roles assigned to user.'),
  );
  $form['password'] = array(
    'title' => t('Password'),
    'description' => t('Affected: password.'),
    'exclude_add' => TRUE,
  );
  $form['contact']  = array(
    'title' => t('Contact'),
    'description' => t('Affected: user contact option.'),
    'exclude_add' => TRUE,
  );

  return $form;
}

/**
 * Implementation of hook_user_import_data().
 */
function user_user_import_data($settings, $update_setting, $column_settings, $module, $field_id, $data, $column_id) {

  if ($module != 'user') {
    return;
  }

  if ($field_id == 'email') {
    $value = trim($data[$column_id]);

    if (!empty($settings['options']['email_domain'])) {
      $value .= $settings['options']['email_domain'];
    }

    _user_import_validate_email($value, $update_setting);
  }

  if ($field_id == 'password') {
    $value = trim($data[$column_id]);
  }

  if ($field_id == 'created') {
    $value = trim($data[$column_id]);

    if (!empty($value) && !is_numeric($value)) {
      $parsed = date_parse($value);
    $timestamp = mktime(
        $parsed['hour'],
        $parsed['minute'],
        $parsed['second'],
        $parsed['month'],
        $parsed['day'],
        $parsed['year']
      );

      if (!empty($timestamp) && is_numeric($timestamp)) {
        $value = $timestamp;
      }
    }
  }

  if ($field_id == 'roles') {
    $value = user_user_import_roles_data($data[$column_id], $settings['roles_new']);
  }

  return $value;
}

/**
 * Implementation of hook_user_import_pre_save().
 */
function user_user_import_pre_save($settings, $account, $fields, $errors, $update_setting_per_module) {

  $account_add['mail'] = $fields['user']['email'][0];

  if (!empty($account->uid)) {
    // Update roles.
    switch ($update_setting_per_module['roles']) {
      case UPDATE_ADD:
        // Include currently assigned roles.
        foreach ($account->roles as $role_name) {
          $account_add['roles'][$role_name] = $role_name;
        }

      case UPDATE_REPLACE:
        // Update roles.
        if (!isset($account_add['roles'])) {
          $account_add['roles'] = array();
        }
        foreach ($settings['roles'] as $role_name => $role_set) {
          if (!empty($role_set)) {
            $account_add['roles'][$role_name] = $role_name;
          }
        }

        break;
    }

    // Update password.
    if ($update_setting_per_module['password'] == UPDATE_REPLACE) {
      $account_add['pass'] = (empty($fields['user']['password'][0])) ? user_password() : $fields['user']['password'][0];
    }
    else {
      $account_add['pass'] = "";
    }

    // Update contact.
    if ($update_setting_per_module['contact'] == UPDATE_REPLACE) {
      $account_add['contact'] = $settings['contact'];
    }
    else {
      $account_add['contact'] = isset($account->contact) ? $account->contact : '';
    }

  }
  else {

    $account_add['timezone'] = config_get('system.date', 'default_timezone');
    $account_add['status']   = 1;
    $account_add['init']     = $fields['user']['email'][0];
    $account_add['pass']     = (empty($fields['user']['password'][0])) ? user_password() : $fields['user']['password'][0];

    if (isset($fields['user']['created'][0]) && !empty($fields['user']['created'][0])) {
      $account_add['created'] = $fields['user']['created'][0];
    }

    $account_add['init'] = $fields['user']['email'][0];

    if (!empty($settings['options']['activate'])) {
      $account_add['access'] = time();
      $account_add['login']  = time();
    }

    // Add selected roles.
    foreach ($settings['roles'] as $role_name => $role_set) {
      if (!empty($role_set)) {
        $account_add['roles'][$role_name] = $role_name;
      }
    }
  }

  return $account_add;
}

/**
 * Implementation of hook_user_import_after_save().
 */
function user_user_import_after_save($settings, $account, $password, $fields, $updated, $update_setting_per_module) {
  /**
   * @todo change hook_user_import_after_save() so that all changes to data are returned and saved in one hit
   */

  $roles = isset($fields['user']['roles']) ? $fields['user']['roles'] : array();

  user_user_import_after_save_role($account, $settings['roles_new'], $account->roles, $roles);

  return;
}

/**
 * Role assign.
 */
function user_user_import_edit_roles_fields(&$form, $import, $collapsed) {
  if (!user_access('user import assign roles')) {
    return;
  }
  $roles_data = user_roles(TRUE);

  // Remove 'authenticated user' option.
  foreach ($roles_data as $role_name => $role_label) {
    if ($role_name != 'authenticated') {
      $roles[$role_name] = $role_label;
    }
  }

  // Roles selected.
  if (!empty($import['roles'])) {
    foreach ($import['roles'] as $role_name) {
      if ($role_name) {
        $roles_selected[] = $role_name;
      }
    }
  }

  if (empty($roles_selected)) {
    $roles_selected[] = 'authenticated';
  }

  $form['role_selection'] = array(
    '#type' => 'fieldset',
    '#title' => t('Role Assign'),
    '#weight' => -80,
    '#collapsible' => TRUE,
    '#collapsed' => $collapsed,
  );

  $form['role_selection']['roles'] = array(
    '#title' => t('Assign Role(s) To All Users'),
    '#type' => 'checkboxes',
    '#options' => $roles,
    '#default_value' => $roles_selected,
    '#description' => t("Select which role(s) all imported users should be assigned. The role 'authenticated user' is assigned automatically."),
  );

  $form['role_selection']['roles_new'] = array(
    '#type' => 'checkbox',
    '#title' => t('Add New Roles'),
    '#default_value' => isset($import['roles_new']) ? $import['roles_new'] : 0,
    '#description' => t('Create imported role(s) that are not found and assign it to the user, in addition to any role(s) selected above. Warning: incorrect roles will be created if the incoming data includes typos.'),
  );

  return;
}

function user_user_import_edit_email_fields(&$form, $import, $collapsed) {

  $form['email_message'] = array(
    '#type' => 'fieldset',
    '#title' => t('Email Message'),
    '#description' => t('Welcome message to be sent to imported users. Leave blank to use the default !message. If an existing user account is updated no welcome email will be sent to that user. <strong>Note - if "Activate Accounts" option is enabled !login_url (one time login) will not work.</strong>', array('!message' => l(t('message'), 'admin/config/people/emails'))),
    '#collapsible' => TRUE,
    '#collapsed' => $collapsed,
  );

  $form['email_message']['subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Message Subject'),
    '#default_value' => isset($import['subject']) ? $import['subject'] : '',
    '#description' => t('Customize the subject of the welcome e-mail, which is sent to imported members.') . ' ' . t('Available variables are:') . ' !username, !site, !password, !uri, !uri_brief, !mailto, !date, !login_uri, !edit_uri, !login_url.',
    '#element_validate' => array('user_import_send_email_subject_validate'),
  );

  $form['email_message']['message'] = array(
    '#type' => 'textarea',
    '#title' => t('Message'),
    '#default_value' => isset($import['message']) ? $import['message'] : '',
    '#description' => t('Customize the body of the welcome e-mail, which is sent to imported members.') . ' ' . t('Available variables are:') . ' !username, !site, !password, !uri, !uri_brief, !mailto, !login_uri, !edit_uri, !login_url.',
    '#element_validate' => array('user_import_send_email_message_validate'),
  );

  $form['email_message']['message_format'] = array(
    '#type' => 'radios',
    '#title' => t('Email Format'),
    '#default_value' => isset($import['message_format']) ? $import['message_format'] : 0,
    '#options' => array(
      0 => t('Plain Text'),
      1 => t('HTML'),
    ),
  );

  // CSS Field only needs to show for HTML format.
  $form['email_message']['message_css'] = array(
    '#type' => 'textarea',
    '#title' => t('CSS'),
    '#default_value' => isset($import['message_css']) ? $import['message_css'] : '',
    '#description' => t('Add CSS styles here if desired, not including &lt;style&gt; or &lt;/style&gt;.'),
    '#states' => array(
      'invisible' => array(
        ':input[name="message_format"]' => array(
          'value' => 0,
        ),
      ),
    ),
  );

  return;
}

function _user_import_validate_email($email = NULL, $duplicates_allowed = FALSE) {

  if (empty($email)) {
    user_import_errors(t('no email'));
  }
  elseif (!valid_email_address($email)) {
    user_import_errors(t('invalid email'));
  }
  // Handle duplicate emails.
  elseif (!$duplicates_allowed && _user_import_existing_uid($email)) {
    user_import_errors(t('duplicate email'));
  }

  return;
}

function user_user_import_roles_data($data, $new_roles_allowed) {
  $roles = array();

  if (empty($data)) {
    return;
  }
  $values = explode(',', $data);

  // Check if any roles are specified that don't already exist.
  $existing_roles = user_roles();

  foreach ($values as $piece) {
    $role         = trim($piece);
    $unrecognised = array();

    if (!empty($role)) {
      // Only add if role is recognized or adding new roles is allowed.
      if (empty($new_roles_allowed) && empty($existing_roles[$role])) {
        $unrecognised[] = $role;
      }
      else {
        $roles[] = $role;
      }
    }
  }

  if (!empty($unrecognised)) {
    user_import_errors(t('The following unrecognised roles were specified: ') . implode(',', $unrecognised));
  }

  return $roles;
}

/**
 * Return an existing user ID, if present, for a given email.
 */
function _user_import_existing_uid($email) {

  $sql  = 'SELECT uid FROM {users} WHERE mail = :mail';
  $args = array(':mail' => $email);
  $uid  = db_query_range($sql, 0, 1, $args)->fetchField();

  return $uid;
}

function user_user_import_after_save_role($account, $new_roles_allowed, $account_roles, $roles) {

  $existing_roles = user_roles();
  // If roles were specified, add to existing roles.
  $assign_roles = array();

  if (is_array($roles) && !empty($roles)) {
    foreach ($roles as $role) {
      if (!empty($role)) {
        if (!empty($new_roles_allowed) && empty($existing_roles[$role])) {
          // Create new role.
          $newrole = new stdClass();
          $newrole->name = $role;
          $newrole->label = $role;
          user_role_save($newrole);
          $existing_roles[$role] = $role;
        }
        $assign_roles[$role] = $role;
      }
    }
    $need_update = FALSE;

    foreach ($assign_roles as $role) {
      if (!in_array($role, $account->roles)) {
        $need_update = TRUE;
        $account->roles[] = $role;
      }
    }

    if ($need_update) {
      user_save($account);
    }
  }

  return;
}
