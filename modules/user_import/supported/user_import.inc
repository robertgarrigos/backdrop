<?php

/**
 * Implements hook_user_import_form_fieldset().
 * Add fieldsets to an import settings form.
 */
function user_import_user_import_form_fieldset($import, $collapsed) {

  $form = array();
  _user_import_edit_template_fields($form, $import);
  _user_import_edit_settings_fields($form, $import, $collapsed);
  _user_import_edit_remove_fields($form, $import);

  return $form;
}

/**
 * Implements hook_user_import_after_save().
 */
function user_import_user_import_after_save($settings, $account, $password, $fields, $updated, $update_setting_per_module) {
  if (!empty($settings['send_email']) && !$updated) {

    $subscribed = isset($settings['subscribed']) ? $settings['subscribed'] : NULL;

    _user_import_send_email($account,
      $password,
      $fields,
      $settings['subject'],
      $settings['message'],
      $settings['message_format'],
      $settings['message_css'],
      $subscribed
    );
  }
}

/**
 * Implements hook_user_import_imported().
 */
function user_import_user_import_imported($import_id, $settings) {
  // Delete file after it's been processed,
  //_user_import_file_deletion($settings['filepath'], $settings['filename'], $settings['oldfilename'], FALSE, FALSE);
  file_unmanaged_delete($settings['filepath']);
}

/**
 * Send email when account is created
 */
function _user_import_send_email($account, $password, $profile, $subject, $body, $format, $css, $subscribed) {
  $config = config('user_import.settings');

  global $base_url;

  // All system mails need to specify the module and template key (mirrored from
  // hook_mail()) that the message they want to send comes from.
  $module = 'user_import';
  $key    = 'welcome';

  // Specify 'to' and 'from' addresses.
  $to   = $account->mail;
  $from = config_get('system.core', 'site_mail');

  $params = array(
    '!username' => $account->name,
    '!uid' => $account->uid,
    '!site' => config_get('system.core', 'site_name'),
    '!login_url' => user_pass_reset_url($account),
    '!password' => $password,
    '!uri' => $base_url,
    '!uri_brief' => backdrop_substr($base_url, backdrop_strlen('http://')),
    '!mailto' => $account->mail,
    '!date' => format_date(time()),
    '!login_uri' => url('user', array('absolute' => TRUE)),
    '!edit_uri' => url('user/' . $account->uid . '/edit', array('absolute' => TRUE)),
    'subject' => $subject,
    'body' => $body,
    'email_format' => $format,
    'css' => $css,
    'user' => $account,
  );

  $language = user_preferred_language($account);

  // Whether or not to automatically send the mail when backdrop_mail() is
  // called. This defaults to TRUE, and is normally what you want unless you
  // need to do additional processing before backdrop_mail_send() is called.
  $send = TRUE;

  $sent = backdrop_mail($module, $key, $to, $language, $params, $from, $send);

  return;
}

function _user_import_edit_settings_fields(&$form, $import, $collapsed) {

  $form['optional'] = array(
    '#type' => 'fieldset',
    '#title' => t('Options'),
    '#weight' => -85,
    '#collapsible' => TRUE,
    '#collapsed' => $collapsed,
  );

  $form['optional']['first_line_skip'] = array(
    '#type' => 'checkbox',
    '#title' => t('Ignore First Line'),
    '#default_value' => isset($import['first_line_skip']) ? $import['first_line_skip'] : 0,
    '#description' => t('If the first line is the names of the data columns, set to ignore first line.'),
  );
  /**
   * @todo move contact options to a separate contact.inc support file
   */
  $form['optional']['contact'] = array(
    '#type' => 'checkbox',
    '#title' => t('Contact'),
    '#default_value' => isset($import['contact']) ? $import['contact'] : 0,
    '#description' => t("Set each user's personal contact form to 'allowed'."),
  );

  $form['optional']['send_email'] = array(
    '#type' => 'checkbox',
    '#title' => t('Send Email'),
    '#default_value' => isset($import['send_email']) ? $import['send_email'] : 0,
    '#description' => t('Send email to users when their account is created.'),
  );

  $form['optional']['username_space'] = array(
    '#type' => 'checkbox',
    '#title' => t('Username Space'),
    '#default_value' => isset($import['username_space']) ? $import['username_space'] : 0,
    '#description' => t("Include spaces in usernames, e.g. 'John' + 'Smith' => 'John Smith'."),
  );

  $form['optional']['activate'] = array(
    '#type' => 'checkbox',
    '#title' => t('Activate Accounts'),
    '#default_value' => isset($import['activate']) ? $import['activate'] : 0,
    '#description' => t("User accounts will not be visible to other users until their owner logs in. Select this option to make all imported user accounts visible. <strong>Note - one time login links in welcome emails will no longer work if this option is enabled.</strong>"),
  );

  $form['optional']['multi_value_delimiter'] = array(
    '#type' => 'textfield',
    '#title' => t('Multi Value Delimiter'),
    '#size' => 4,
    '#default_value' => isset($import['multi_value_delimiter']) ? $import['multi_value_delimiter'] : '*||*',
    '#description' => t("Character(s) to use to split data so that it can be added to a field set to multiple values, e.g. '||', '*||*'"),
  );

  $form['optional']['email_domain'] = array(
    '#type' => 'textfield',
    '#title' => t('Email Domain'),
    '#description' => t("Create an email address with the combined contents of Email Address (in Field Match above) and this field. <em><br />For example if Email Address is 'Luthor' and Email Domain is '@lexcorp.com', the account address woud be 'luthor@lexcorp.com'. <br />Note that '@example.com' is recommended for dummy addresses.</em>"),
    '#default_value' => isset($import['email_domain']) ? $import['email_domain'] : '',
  );

  $delimiter = isset($import['delimiter']) ? $import['delimiter'] : ',';
  $encoding = isset($import['encoding']) ? $import['encoding'] : 'UTF-8';

  $form['optional']['delimiter'] = array(
    '#type'          => 'hidden',
    '#value'         => $delimiter,
  );

  $form['optional']['encoding'] = array(
    '#type'          => 'hidden',
    '#value'         => $encoding,
  );

  $form['filesettings'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('File Settings'),
    '#weight'      => -84,
    '#collapsible' => TRUE,
    '#collapsed'   => $collapsed,
  );

  // These should not be options in this form.
  $form['filesettings']['delimiter'] = array(
    '#markup' => t('File Delimiter: "') . $delimiter . '".<br />' . t('Character Encoding') . ': "' . $encoding . '".</p>',
  );

  return;
}

function _user_import_edit_template_fields(&$form, $import) {
  $config = config('user_import.settings');

  // Settings template update controls.
  if (empty($import['name'])) {

    // New settings template save controls.

    $form['save'] = array(
      '#type' => 'fieldset',
      '#title' => t('Save Settings'),
      '#description' => t('Save settings for re-use on other imports.'),
      '#weight' => 90,
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    );

    $form['save']['name'] = array(
      '#type' => 'textfield',
      '#title' => t('Settings Name'),
      '#size' => 26,
      '#maxlength' => 25,
      '#description' => t('Name to identify these settings by.'),
    );

    $auto_imports_enabled = $config->get('auto_imports_enabled');

    if (!empty($auto_imports_enabled)) {
      $form['save']['auto_import_directory'] = array(
        '#type' => 'textfield',
        '#title' => t('Auto Import Directory Name'),
        '#description' => t('If this is set a directory with this name will be created, into which files can be uploaded and automatically processed, using the settings on this page to create new user accounts.'),
        '#default_value' => isset($import['auto_import_directory']) ? $import['auto_import_directory'] : '',
      );
    }

    $form['save'][] = array(
      '#type' => 'submit',
      '#value' => t('Save'),
      '#validate' => array(
        'user_import_template_has_name_validate',
        'user_import_template_unique_name_validate',
        'user_import_edit_validate',
      ),
      '#submit' => array('user_import_template_new_submit'),
    );

  }
  else {

    $form['save'] = array(
      '#type' => 'fieldset',
      '#title' => t('Saved Settings'),
      '#description' => t("If changes have been made to the settings since they where last saved you can update the saved template, or save them as a new template."),
      '#weight' => 90,
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );

    $form['#current_template'] = $import['name'];

    $form['save']['update'] = array(
      '#type' => 'fieldset',
      '#title' => t('Update'),
      '#description' => t("Update '%name' settings template", array('%name' => $import['name'])),
    );

    $auto_imports_enabled = $config->get('auto_imports_enabled');

    if (!empty($auto_imports_enabled)) {
      $form['save']['auto_import_directory'] = array(
        '#type' => 'textfield',
        '#title' => t('Auto Import Directory Name'),
        '#description' => t('If this is set a directory with this name will be created, into which files can be uploaded and automatically processed, using the settings on this page to create new user accounts.'),
        '#default_value' => isset($import['auto_import_directory']) ? $import['auto_import_directory'] : '',
      );
    }

    $form['save']['update']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Update'),
      '#validate' => array('user_import_edit_validate'),
      '#submit' => array('user_import_template_update_submit'),
    );

    $form['save']['new'] = array(
      '#type' => 'fieldset',
      '#title' => t('Create New'),
      '#description' => t("Save as new settings template"),
    );

    $form['save']['new']['name'] = array(
      '#type' => 'textfield',
      '#title' => t('Save As New'),
      '#size' => 30,
      '#maxlength' => 25,
      '#description' => t('Name to identify these settings by.'),
    );

    $auto_imports_enabled = $config->get('auto_imports_enabled');

    if (!empty($auto_imports_enabled)) {
      $form['save']['auto_import_directory'] = array(
        '#type' => 'textfield',
        '#title' => t('Auto Import Directory Name'),
        '#description' => t('If this is set a directory with this name will be created, into which files can be uploaded and automatically processed, using the settings on this page to create new user accounts.'),
        '#default_value' => isset($import['auto_import_directory']) ? $import['auto_import_directory'] : '',
      );
    }

    $form['save']['new'][] = array(
      '#type' => 'submit',
      '#value' => t('Save As New'),
      '#validate' => array('user_import_template_has_name_validate', 'user_import_template_unique_name_validate', 'user_import_edit_validate'),
      '#submit' => array('user_import_template_new_submit'),
    );
  }

  return;
}

/**
 * Validate that a template has a name.
 */
function user_import_template_has_name_validate($form, &$form_state) {
  $template_name = trim($form_state['values']['name']);
  if (empty($template_name)) {
    form_set_error('name', t('A name needs to be set to save this settings template.'));
  }
}

/**
 * Validate that a template has a unique name.
 */
function user_import_template_unique_name_validate($form, &$form_state) {
  $template_name = trim($form_state['values']['name']);
  $unique_name   = db_query('SELECT COUNT(import_id) FROM {user_import} WHERE name = :name', array(':name' => $template_name))->fetchField();
  if (!empty($unique_name)) {
    form_set_error('name', t("'!name' is already in use by another settings template.", array('!name' => $template_name)));
  }
}

/**
 * Validate that a email subject line has been set if Send Email is enabled.
 */
function user_import_send_email_subject_validate($element, &$form_state) {
  if (!empty($form_state['values']['send_email']) && (empty($form_state['values']['subject']) && !empty($form_state['values']['message']))) {
    form_error($element, t('If Send Email has been enabled then either both <strong>email subject</strong> and <strong>email message</strong> must be set, or none of them.'));
  }
}

/**
 * Validate that a email message has been set if Send Email is enabled.
 */
function user_import_send_email_message_validate($element, &$form_state) {
  if (!empty($form_state['values']['send_email']) && (empty($form_state['values']['message']) && !empty($form_state['values']['subject']))) {
    form_error($element, t('If Send Email has been enabled then either both <strong>email subject</strong> and <strong>email message</strong> must be set, or none of them.'));
  }
}

function _user_import_edit_remove_fields(&$form, $import) {

  $form['remove'] = array(
    '#type' => 'fieldset',
    '#title' => t('Use Different CSV File'),
    '#description' => t('Remove file to use a different file. All settings on this page will be deleted.'),
    '#weight' => -100,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['remove']['file'] = array(
    '#type' => 'item',
    '#title' => t('Uploaded file: @filename', array('@filename' => $import['filename'])),
    '#value' => $import['filename'],
  );

  $form['remove']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Remove file'),
    '#validate' => array('user_import_edit_remove_file_validate'),
  );

  return;
}

/**
 *  Delete settings and uploaded file
 */
function user_import_edit_remove_file_validate($form, &$form_state) {

  $settings = _user_import_settings_select($form_state['values']['import_id']);
  _user_import_settings_deletion($form_state['values']['import_id']);
  //_user_import_file_deletion($settings['filepath'], $settings['filename'], $settings['oldfilename'], $settings['options']['ftp']);
  file_unmanaged_delete($settings['filepath']);
  backdrop_goto('admin/people/user_import/add');
}
