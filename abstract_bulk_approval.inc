<?php

function om_flowsheet_abstract_bulk_approval_form($form, &$form_state)
{
	$options_first = _bulk_list_of_flowsheet_project();
	$selected = isset($form_state['values']['flowsheet_project']) ? $form_state['values']['flowsheet_project'] : key($options_first);
	$form = array();
	$form['flowsheet_project'] = array(
		'#type' => 'select',
		'#title' => t('Title of the flowsheeting project'),
		'#options' => _bulk_list_of_flowsheet_project(),
		'#default_value' => $selected,
		'#ajax' => array(
			
			'callback' => 'ajax_bulk_flowsheet_abstract_details_callback'
		),
		'#suffix' => '<div id="ajax_selected_flowsheet"></div><div id="ajax_selected_flowsheet_pdf"></div>'
	);
	$form['flowsheet_actions'] = array(
		'#type' => 'select',
		'#title' => t('Please select action for Flowsheeting project'),
		'#options' => _bulk_list_flowsheet_actions(),
		'#default_value' => 0,
		'#prefix' => '<div id="ajax_selected_flowsheet_action" style="color:red;">',
		'#suffix' => '</div>',
		'#states' => array(
			'invisible' => array(
				':input[name="flowsheet_project"]' => array(
					'value' => 0
				)
			)
		)
	);
	$form['disapprove_message'] = array(
		'#type' => 'textarea',
		'#title' => t('Enter the reason for disapproving the proposal<span style="color: red;">*</span>'),
		'#prefix' => '<div id= "message_submit">',
		'#states' => array(
			'visible' => array(
				array(
					':input[name="flowsheet_actions"]' => array(
						'value' => 2
					)
				)
			)
		)
	);
	$form['deletion_message'] = array(
		'#type' => 'textarea',
		'#title' => t('Enter the reason for deleting the proposal'),
		'#prefix' => '<div id= "message_submit">',
		'#states' => array(
			'visible' => array(
				array(
					':input[name="flowsheet_actions"]' => array(
						'value' => 3
					)
				)
			)
		)
	);
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Submit'),
		'#states' => array(
			'invisible' => array(
				':input[name="lab"]' => array(
					'value' => 0
				)
			)
		)
	);
	return $form;
}
function ajax_bulk_flowsheet_abstract_details_callback($form, $form_state)
{
	$commands = array();
	$flowsheet_project_default_value = $form_state['values']['flowsheet_project'];
	if ($flowsheet_project_default_value != 0)
	{
		$commands[] = ajax_command_html('#ajax_selected_flowsheet', _flowsheet_details($flowsheet_project_default_value));
		$form['flowsheet_actions']['#options'] = _bulk_list_flowsheet_actions();
		$commands[] = ajax_command_replace('#ajax_selected_flowsheet_action', drupal_render($form['flowsheet_actions']));
	} //$flowsheet_project_default_value != 0
	else
	{
		$commands[] = ajax_command_html('#ajax_selected_flowsheet', '');
		$commands[] = ajax_command_data('#ajax_selected_flowsheet', 'form_state_value_select', $form_state['values']['flowsheet_project']);
	}
	return array(
		'#type' => 'ajax',
		'#commands' => $commands
	);
}

/************************************************************/
function om_flowsheet_abstract_bulk_approval_form_submit($form, &$form_state)
{
	global $user;
	$msg = '';
	$root_path = om_flowsheet_document_path();
	if ($form_state['clicked_button']['#value'] == 'Submit')
	{
		if ($form_state['values']['flowsheet_project'])
		// om_flowsheet_abstract_del_lab_pdf($form_state['values']['flowsheet_project']);
			if (user_access('om flowsheet bulk manage abstract'))
			{
				$query = db_select('om_flowsheet_proposal');
				$query->fields('om_flowsheet_proposal');
				$query->condition('id', $form_state['values']['flowsheet_project']);
				$user_query = $query->execute();
				$user_info = $user_query->fetchObject();
				$user_data = user_load($user_info->uid);
				if ($form_state['values']['flowsheet_actions'] == 1)
				{
					// approving entire project //
					$query = db_select('om_flowsheet_submitted_abstracts');
					$query->fields('om_flowsheet_submitted_abstracts');
					$query->condition('proposal_id', $form_state['values']['flowsheet_project']);
					$abstracts_q = $query->execute();
					$experiment_list = '';
					while ($abstract_data = $abstracts_q->fetchObject())
					{
						db_query("UPDATE {om_flowsheet_submitted_abstracts} SET abstract_approval_status = 1, approver_uid = :approver_uid WHERE id = :id", array(
							':approver_uid' => $user->uid,
							':id' => $abstract_data->id
						));
						db_query("UPDATE {om_flowsheet_submitted_abstracts_file} SET file_approval_status = 1, approvar_uid = :approver_uid WHERE submitted_abstract_id = :submitted_abstract_id", array(
							':approver_uid' => $user->uid,
							':submitted_abstract_id' => $abstract_data->id
						));
					} //$abstract_data = $abstracts_q->fetchObject()
					drupal_set_message(t('Approved Flowsheeting project.'), 'status');
					// email 
					$email_subject = t('[!site_name][Flowsheeting Project] Your uploaded OpenModelica Flowsheeting Project has been approved', array(
						'!site_name' => variable_get('site_name', '')
					));
					$email_body = array(
						0 => t('

Dear ' . $user_info->contributor_name . ',

Congratulations!
Your OpenModelica flowsheet and abstract with the following details have been approved.

Full Name: ' . $user_info->name_title . ' ' . $user_info->contributor_name . '
Project Title: ' . $user_info->project_title . '
Name of compound for which process development is carried out: ' . $user_info->process_development_compound_name . '

Kindly send us the internship forms available at https://om.fossee.in/chemical/flowsheeting-project/internship/forms as early as possible for processing your honorarium on time. In case you have already sent these forms, please share the the consignment number or tracking id with us.

Note: It will take upto 45 days from the time we receive your forms, to process your honorarium.

Best Wishes,

!site_name Team
FOSSEE, IIT Bombay', array(
							'!site_name' => variable_get('site_name', ''),
							'!user_name' => $user_data->name
						))
					);
					/** sending email when everything done **/
					$email_to = $user_data->mail;
					$from = variable_get('om_flowsheet_from_email', '');
					$bcc = variable_get('om_flowsheet_emails', '');
					$cc = variable_get('om_flowsheet_cc_emails', '');
					$params['standard']['subject'] = $email_subject;
					$params['standard']['body'] = $email_body;
					$params['standard']['headers'] = array(
						'From' => $from,
						'MIME-Version' => '1.0',
						'Content-Type' => 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
						'Content-Transfer-Encoding' => '8Bit',
						'X-Mailer' => 'Drupal',
						'Cc' => $cc,
						'Bcc' => $bcc
					);
					if (!drupal_mail('om_flowsheet', 'standard', $email_to, language_default(), $params, $from, TRUE))
					{
						$msg = drupal_set_message('Error sending email message.', 'error');
					} //!drupal_mail('om_flowsheet', 'standard', $email_to, language_default(), $params, $from, TRUE)
				} //$form_state['values']['flowsheet_actions'] == 1
				elseif ($form_state['values']['flowsheet_actions'] == 2)
				{
					//pending review entire project 
					if(strlen(trim($form_state['values']['disapprove_message'])) == 0)
					{
						form_set_error('disapprove_message', t(''));
						$msg = drupal_set_message("Please mention the reason for disapproval.", 'error');
						return $msg;
					}
					else if (strlen(trim($form_state['values']['disapprove_message'])) <= 30)
					{
						form_set_error('disapprove_message', t(''));
						$msg = drupal_set_message("Please mention the reason for disapproval. Minimum 30 character required", 'error');
						return $msg;
					}
					$query = db_select('om_flowsheet_submitted_abstracts');
					$query->fields('om_flowsheet_submitted_abstracts');
					$query->condition('proposal_id', $form_state['values']['flowsheet_project']);
					$abstracts_q = $query->execute();
					$experiment_list = '';
					while ($abstract_data = $abstracts_q->fetchObject())
					{
						db_query("UPDATE {om_flowsheet_submitted_abstracts} SET abstract_approval_status = 0,  is_submitted = 0, approver_uid = :approver_uid WHERE id = :id", array(
							':approver_uid' => $user->uid,
							':id' => $abstract_data->id
						));
						db_query("UPDATE {om_flowsheet_submitted_abstracts_file} SET file_approval_status = 0, approvar_uid = :approver_uid WHERE submitted_abstract_id = :submitted_abstract_id", array(
							':approver_uid' => $user->uid,
							':submitted_abstract_id' => $abstract_data->id
						));
					} //$abstract_data = $abstracts_q->fetchObject()
					drupal_set_message(t('Disapproved the submission.'), 'status');
					// email 
					$email_subject = t('[!site_name][Flowsheeting Project] Your uploaded OpenModelica Flowsheeting Project has been marked as disapproved', array(
						'!site_name' => variable_get('site_name', '')
					));
					$email_body = array(
						0 => t('

Dear ' . $user_info->contributor_name . ',

We regret to inform you that your OpenModelica flowsheet and abstract with the following details have been disapproved:

Full Name: ' . $user_info->name_title . ' ' . $user_info->contributor_name . '
Project Title: ' . $user_info->project_title . '
Name of compound for which process development is carried out: ' . $user_info->process_development_compound_name . '
Reason for Disapproval / Feedback: ' . $form_state['values']['disapprove_message'] . ' 
 
You are requested to visit the Abstract and Flowsheet submission page at https://om.fossee.in/chemical/flowsheeting-project/abstract-code and re-upload the flowsheet and abstract.

Best Wishes,

!site_name Team
FOSSEE, IIT Bombay', array(
							'!site_name' => variable_get('site_name', ''),
							'!user_name' => $user_data->name
						))
					);
					/** sending email when everything done **/
					$email_to = $user_data->mail;
					$from = variable_get('om_flowsheet_from_email', '');
					$bcc = variable_get('om_flowsheet_emails', '');
					$cc = variable_get('om_flowsheet_cc_emails', '');
					$params['standard']['subject'] = $email_subject;
					$params['standard']['body'] = $email_body;
					$params['standard']['headers'] = array(
						'From' => $from,
						'MIME-Version' => '1.0',
						'Content-Type' => 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
						'Content-Transfer-Encoding' => '8Bit',
						'X-Mailer' => 'Drupal',
						'Cc' => $cc,
						'Bcc' => $bcc
					);
					if (!drupal_mail('om_flowsheet', 'standard', $email_to, language_default(), $params, $from, TRUE))
					{
						drupal_set_message('Error sending email message.', 'error');
					} //!drupal_mail('om_flowsheet', 'standard', $email_to, language_default(), $params, $from, TRUE)
				} //$form_state['values']['flowsheet_actions'] == 2
				elseif ($form_state['values']['flowsheet_actions'] == 3) //disapprove and delete entire flowsheeting project
				{
					if(strlen(trim($form_state['values']['deletion_message'])) == 0)
					{
						form_set_error('deletion_message', t(''));
						$msg = drupal_set_message("Please mention the reason for deletion.", 'error');
						return $msg;
					}
					else if (strlen(trim($form_state['values']['deletion_message'])) <= 30)
					{
						form_set_error('message', t(''));
						$msg = drupal_set_message("Please mention the reason for deletion. Minimum 30 character required", 'error');
						return $msg;
					} //strlen(trim($form_state['values']['message'])) <= 30
					if (!user_access('om flowsheet bulk delete code'))
					{
						$msg = drupal_set_message(t('You do not have permission to Bulk Dis-Approved and Deleted Entire Lab.'), 'error');
						return $msg;
					} //!user_access('flowsheet bulk delete code')
					if (om_flowsheet_abstract_delete_project($form_state['values']['flowsheet_project'])) //////
					{
						drupal_set_message(t('Dis-Approved and Deleted Entire Flowsheeting project.'), 'status');
						$email_subject = t('[!site_name][Flowsheeting Project] Your uploaded OpenModelica Flowsheeting Project has been deleted with the proposal form', array(
						'!site_name' => variable_get('site_name', '')
						));
						$email_body = array(
						0 => t('

Dear ' . $user_info->contributor_name . ',

We regret to inform you that your OpenModelica flowsheet and abstract along with the proposal form with the following details have been deleted:

Full Name: ' . $user_info->name_title . ' ' . $user_info->contributor_name . '
Project Title: ' . $user_info->project_title . '
Name of compound for which process development is carried out: ' . $user_info->process_development_compound_name . '
Reason for dis-approval: ' . $form_state['values']['deletion_message'] . '

Now, you can propose a new flowsheet.

Best Wishes,

!site_name Team
FOSSEE, IIT Bombay', array(
					'!site_name' => variable_get('site_name', ''),
					'!user_name' => $user_data->name
					))
						);
					$email_to = $user_data->mail;
					$from = variable_get('om_flowsheet_from_email', '');
					$bcc = variable_get('om_flowsheet_emails', '');
					$cc = variable_get('om_flowsheet_cc_emails', '');
					$params['standard']['subject'] = $email_subject;
					$params['standard']['body'] = $email_body;
					$params['standard']['headers'] = array(
						'From' => $from,
						'MIME-Version' => '1.0',
						'Content-Type' => 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
						'Content-Transfer-Encoding' => '8Bit',
						'X-Mailer' => 'Drupal',
						'Cc' => $cc,
						'Bcc' => $bcc
					);
					if (!drupal_mail('om_flowsheet', 'standard', $email_to, language_default(), $params, $from, TRUE))
					{
						drupal_set_message('Error sending email message.', 'error');
					} 
					} //om_flowsheet_abstract_delete_project($form_state['values']['flowsheet_project'])
				else
					{
						drupal_set_message(t('Error Dis-Approving and Deleting Entire flowsheeting project.'), 'error');
					}
				}//$form_state['values']['flowsheet_actions'] == 3
				 //$form_state['values']['flowsheet_actions'] == 4
				
			} //user_access('flowsheet project bulk manage code')
		return $msg;
	} //$form_state['clicked_button']['#value'] == 'Submit'
}
/**********************************************************/
function _bulk_list_of_flowsheet_project()
{
	$project_titles = array(
		'0' => 'Please select...'
	);
	$query = db_select('om_flowsheet_proposal');
	$query->fields('om_flowsheet_proposal');
	$query->condition('approval_status', 1);
	$query->orderBy('project_title', 'ASC');
	$project_titles_q = $query->execute();
	while ($project_titles_data = $project_titles_q->fetchObject())
	{
		$project_titles[$project_titles_data->id] = $project_titles_data->project_title . ' (Proposed by ' . $project_titles_data->contributor_name . ')';
	} //$project_titles_data = $project_titles_q->fetchObject()
	return $project_titles;
}
function _bulk_list_flowsheet_actions()
{
	$flowsheet_actions = array(
		0 => 'Please select...'
	);
	$flowsheet_actions[1] = 'Approve entire flowsheeting project(This action will approve the uploaded flowsheet and abstract)';
	$flowsheet_actions[2] = 'Disapprove entire flowsheeting project(Enables the submission interface for the user(Files will not be deleted but replaced if user re-uploads)';
	$flowsheet_actions[3] = 'Delete entire flowsheeting project including proposal(This action will delete the entire project files including the proposal.)';
	//$flowsheet_actions[4] = 'Delete Entire Flowsheeting Project Including Proposal';
	return $flowsheet_actions;
}
function _flowsheet_details($flowsheet_proposal_id)
{
	$return_html = "";
	$query_pro = db_select('om_flowsheet_proposal');
	$query_pro->fields('om_flowsheet_proposal');
	$query_pro->condition('id', $flowsheet_proposal_id);
	$abstracts_pro = $query_pro->execute()->fetchObject();
	$query_pdf = db_select('om_flowsheet_submitted_abstracts_file');
	$query_pdf->fields('om_flowsheet_submitted_abstracts_file');
	$query_pdf->condition('proposal_id', $flowsheet_proposal_id);
	$query_pdf->condition('filetype', 'A');
	$abstracts_pdf = $query_pdf->execute()->fetchObject();
	if ($abstracts_pdf == TRUE)
	{
		if ($abstracts_pdf->filename != "NULL" || $abstracts_pdf->filename != "")
		{
			$abstract_filename = $abstracts_pdf->filename;
		} //$abstracts_pdf->filename != "NULL" || $abstracts_pdf->filename != ""
		else
		{
			$abstract_filename = "File not uploaded";
		}
	} //$abstracts_pdf == TRUE
	else
	{
		$abstract_filename = "File not uploaded";
	}
	/*$query_sp = db_select('om_flowsheet_submitted_abstracts_file');
	$query_sp->fields('om_flowsheet_submitted_abstracts_file');
	$query_sp->condition('proposal_id', $flowsheet_proposal_id);
	$query_sp->condition('filetype', 'P');
	$abstracts_sp = $query_sp->execute()->fetchObject();
	if ($abstracts_sp == TRUE)
	{
		if ($abstracts_sp->filename != "NULL" || $abstracts_sp->filename != "")
		{
			$sp_filename = $abstracts_sp->filename;
		} //$abstracts_pdf->filename != "NULL" || $abstracts_pdf->filename != ""
		else
		{
			$sp_filename = "File not uploaded";
		}
	} //$abstracts_pdf == TRUE
	else
	{
		$sp_filename = "File not uploaded";
	}*/
	$query_process = db_select('om_flowsheet_submitted_abstracts_file');
	$query_process->fields('om_flowsheet_submitted_abstracts_file');
	$query_process->condition('proposal_id', $flowsheet_proposal_id);
	$query_process->condition('filetype', 'S');
	$abstracts_query_process = $query_process->execute()->fetchObject();
	$query = db_select('om_flowsheet_submitted_abstracts');
	$query->fields('om_flowsheet_submitted_abstracts');
	$query->condition('proposal_id', $flowsheet_proposal_id);
	$abstracts_q = $query->execute()->fetchObject();
	if ($abstracts_q)
	{
		if ($abstracts_q->is_submitted == 0)
		{
			//drupal_set_message(t('Abstract is not submmited yet.'), 'error', $repeat = FALSE);
			//return;
		} //$abstracts_q->is_submitted == 0
	} //$abstracts_q
	//var_dump($abstracts_query_process);die;
	if ($abstracts_query_process == TRUE)
	{
		if ($abstracts_query_process->filename != "NULL" || $abstracts_query_process->filename != "")
		{
			$abstracts_query_process_filename = $abstracts_query_process->filename;
		} //$abstracts_query_process->filename != "NULL" || $abstracts_query_process->filename != ""
		else
		{
			$abstracts_query_process_filename = "File not uploaded";
		}
		if ($abstracts_q->unit_operations_used_in_om == '')
		{
			$unit_operations_used_in_om = "Not entered";
		} //$abstracts_q->unit_operations_used_in_om == ''
		else
		{
			$unit_operations_used_in_om = $abstracts_q->unit_operations_used_in_om;
		}
		if ($abstracts_q->thermodynamic_packages_used == '')
		{
			$thermodynamic_packages_used = "Not entered";
		} //$abstracts_q->thermodynamic_packages_used == ''
		else
		{
			$thermodynamic_packages_used = $abstracts_q->thermodynamic_packages_used;
		}
		
	} //$abstracts_query_process == TRUE
	else
	{
		$url = l('Upload abstract', 'chemical/flowsheeting-project/abstract-code/upload');
		$unit_operations_used_in_om = "Not entered";
		$thermodynamic_packages_used = "Not entered";
		//$sp_filename = "File not uploaded";
		$abstracts_query_process_filename = "File not uploaded";
	}
	$headers = array(
		"Name of compound for which process development is carried out",
		"CAS No."
	);
	$rows = array();
	$item = array(
		"{$abstracts_pro->process_development_compound_name}",
		"{$abstracts_pro->process_development_compound_cas_number}"
	);
	array_push($rows, $item);
	$prodata = theme('table', array(
		'header' => $headers,
		'rows' => $rows
	));
	$query = db_select('om_flowsheet_library');
	$query->fields('om_flowsheet_library');
	$query->condition('id', $abstracts_pro->simulator_version_id);
	$result = $query->execute()->fetchObject();
	$simulator_version_used = l($result->simulator_version_name, $result->link);
	$download_flowsheet = l('Download flowsheet project','chemical/flowsheeting-project/full-download/project/'.$flowsheet_proposal_id);
	$return_html .= '<strong>Proposer Name:</strong><br />' . $abstracts_pro->name_title . ' ' . $abstracts_pro->contributor_name . '<br /><br />';
	$return_html .= '<strong>Title of the Flowsheet Project:</strong><br />' . $abstracts_pro->project_title . '<br /><br />';
	$return_html .= '<strong>OpenModelica version:</strong><br />' . $abstracts_pro->version . '<br /><br />';
	$return_html .= '<strong>Unit Operations used in OpenModelica:</strong><br />' . $unit_operations_used_in_om . '<br /><br />';
	$return_html .= '<strong>Thermodynamic Packages Used:</strong><br />' . $thermodynamic_packages_used . '<br /><br />';
	$return_html .= '<strong>Name of compound for which process development is carried out:</strong><br />' . $prodata;
	$return_html .= '<strong>Uploaded an abstract (brief outline) of the project:</strong><br />' . $abstract_filename . '<br /><br />';
	$return_html .= '<strong>Upload the OpenModelica flowsheet for the developed process:</strong><br />' . $abstracts_query_process_filename . '<br /><br />';
	$return_html .= '<strong>Simulator version used for creating the flowsheet:</strong><br />' . $simulator_version_used . '<br /><br />';
	$return_html .= $download_flowsheet;
	return $return_html;
}
