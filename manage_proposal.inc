<?php
// $Id$
function om_flowsheet_proposal_pending()
{
	/* get pending proposals to be approved */
	$pending_rows = array();
	//$pending_q = db_query("SELECT * FROM {om_flowsheet_proposal} WHERE approval_status = 0 ORDER BY id DESC");
	$query = db_select('om_flowsheet_proposal');
	$query->fields('om_flowsheet_proposal');
	$query->condition('approval_status', 0);
	$query->orderBy('id', 'DESC');
	$pending_q = $query->execute();
	while ($pending_data = $pending_q->fetchObject())
	{
		$pending_rows[$pending_data->id] = array(
			date('d-m-Y', $pending_data->creation_date),
			l($pending_data->name_title . ' ' . $pending_data->contributor_name, 'user/' . $pending_data->uid),
			$pending_data->project_title,
			l('Approve', 'chemical/flowsheeting-project/manage-proposal/approve/' . $pending_data->id) . ' | ' . l('Edit', 'chemical/flowsheeting-project/manage-proposal/edit/' . $pending_data->id)
		);
	} //$pending_data = $pending_q->fetchObject()
	/* check if there are any pending proposals */
	if (!$pending_rows)
	{
		drupal_set_message(t('There are no pending proposals.'), 'status');
		return '';
	} //!$pending_rows
	$pending_header = array(
		'Date of Submission',
		'Student Name',
		'Title of the Flowsheet Project',
		'Action'
	);
	//$output = theme_table($pending_header, $pending_rows);
	$output = theme('table', array(
		'header' => $pending_header,
		'rows' => $pending_rows
	));
	return $output;
}
function om_flowsheet_proposal_pending_solution()
{
	/* get pending proposals to be approved */
	$pending_rows = array();
	//$pending_q = db_query("SELECT * FROM {om_flowsheet_proposal} WHERE approval_status = 1 ORDER BY id DESC");
	$query = db_select('om_flowsheet_proposal');
	$query->fields('om_flowsheet_proposal');
	$query->condition('approval_status', 1);
	$query->orderBy('id', 'DESC');
	$pending_q = $query->execute();
	while ($pending_data = $pending_q->fetchObject())
	{
		$pending_rows[$pending_data->id] = array(
			date('d-m-Y', $pending_data->creation_date),
			date('d-m-Y', $pending_data->approval_date),
			l($pending_data->name, 'user/' . $pending_data->uid),
			$pending_data->project_title,
			$pending_data->department,
			l('Status', 'chemical/flowsheeting-project/manage-proposal/status/' . $pending_data->id)
		);
	} //$pending_data = $pending_q->fetchObject()
	/* check if there are any pending proposals */
	if (!$pending_rows)
	{
		drupal_set_message(t('There are no proposals pending for solutions.'), 'status');
		return '';
	} //!$pending_rows
	$pending_header = array(
		'Date of Submission',
		'Date of Approval',
		'Student name',
		'Title of the Flowsheet Project',
		'Department',
		'Action'
	);
	$output = theme('table', array(
		'header' => $pending_header,
		'rows' => $pending_rows
	));
	return $output;
}
function om_flowsheet_proposal_all()
{
	/* get pending proposals to be approved */
	$proposal_rows = array();
	//$proposal_q = db_query("SELECT * FROM {om_flowsheet_proposal} ORDER BY id DESC");
	$query = db_select('om_flowsheet_proposal');
	$query->fields('om_flowsheet_proposal');
	$query->orderBy('id', 'DESC');
	$proposal_q = $query->execute();
	while ($proposal_data = $proposal_q->fetchObject())
	{
		$approval_status = '';
		switch ($proposal_data->approval_status)
		{
			case 0:
				$approval_status = 'Pending';
				break;
			case 1:
				$approval_status = 'Approved';
				break;
			case 2:
				$approval_status = 'Dis-approved';
				break;
			case 3:
				$approval_status = 'Completed';
				break;
			default:
				$approval_status = 'Unknown';
				break;
		} //$proposal_data->approval_status
		if ($proposal_data->actual_completion_date == 0)
		{
			$actual_completion_date = "Not Completed";
		} //$proposal_data->actual_completion_date == 0
		else
		{
			$actual_completion_date = date('d-m-Y', $proposal_data->actual_completion_date);
		}
		$proposal_rows[] = array(
			date('d-m-Y', $proposal_data->creation_date),
			l($proposal_data->contributor_name, 'user/' . $proposal_data->uid),
			$proposal_data->project_title,
			$actual_completion_date,
			$approval_status,
			l('Status', 'chemical/flowsheeting-project/manage-proposal/status/' . $proposal_data->id) . ' | ' . l('Edit', 'chemical/flowsheeting-project/manage-proposal/edit/' . $proposal_data->id)
		);
	} //$proposal_data = $proposal_q->fetchObject()
	/* check if there are any pending proposals */
	if (!$proposal_rows)
	{
		drupal_set_message(t('There are no proposals.'), 'status');
		return '';
	} //!$proposal_rows
	$proposal_header = array(
		'Date of Submission',
		'Student Name',
		'Title of the Lab',
		'Date of Completion',
		'Status',
		'Action'
	);
	$output = theme('table', array(
		'header' => $proposal_header,
		'rows' => $proposal_rows
	));
	return $output;
}
/******************************************************************************/
/************************** PROPOSAL APPROVAL FORM ****************************/
/******************************************************************************/
function om_flowsheet_proposal_approval_form($form, &$form_state)
{
	global $user;
	/* get current proposal */
	$proposal_id = (int) arg(4);
	//$proposal_q = db_query("SELECT * FROM {om_flowsheet_proposal} WHERE id = %d", $proposal_id);
	$query = db_select('om_flowsheet_proposal');
	$query->fields('om_flowsheet_proposal');
	$query->condition('id', $proposal_id);
	$proposal_q = $query->execute();
	if ($proposal_q)
	{
		if ($proposal_data = $proposal_q->fetchObject())
		{
			/* everything ok */
		} //$proposal_data = $proposal_q->fetchObject()
		else
		{
			drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
			drupal_goto('chemical/flowsheeting-project/manage-proposal');
			return;
		}
	} //$proposal_q
	else
	{
		drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
		drupal_goto('chemical/flowsheeting-project/manage-proposal');
		return;
	}
	if ($proposal_data->project_guide_name == "NULL" || $proposal_data->project_guide_name == "")
	{
		$project_guide_name = "Not Entered";
	} //$proposal_data->project_guide_name == NULL
	else
	{
		$project_guide_name = $proposal_data->project_guide_name;
	}
	if ($proposal_data->project_guide_email_id == "NULL" || $proposal_data->project_guide_email_id == "")
	{
		$project_guide_email_id = "Not Entered";
	} //$proposal_data->project_guide_email_id == NULL
	else
	{
		$project_guide_email_id = $proposal_data->project_guide_email_id;
	}
	$query = db_select('om_flowsheet_library');
	$query->fields('om_flowsheet_library');
	$query->condition('id', $proposal_data->simulator_version_id);
	$result = $query->execute()->fetchObject();
	$simulator_version_name = l($result->simulator_version_name, $result->link);
	$form['contributor_name'] = array(
		'#type' => 'item',
		'#markup' => l($proposal_data->name_title . ' ' . $proposal_data->contributor_name, 'user/' . $proposal_data->uid),
		'#size' => 250,
		'#title' => t('Student name')
	);
	$form['student_email_id'] = array(
		'#title' => t('Student Email'),
		'#type' => 'item',
		'#markup' => user_load($proposal_data->uid)->mail,
		'#title' => t('Email')
	);
	$form['contributor_contact_no'] = array(
		'#title' => t('Contact No.'),
		'#type' => 'item',
		'#markup' => $proposal_data->contact_no,
	);
	$form['month_year_of_degree'] = array(
		'#type' => 'date_popup',
		'#title' => t('Month and year of award of degree'),
		'#date_label_position' => '',
		'#description' => '',
		'#default_value' => $proposal_data->month_year_of_degree,
		'#date_format' => 'M-Y',
		'#date_increment' => 0,
		'#date_year_range' => '1960:+0',
		'#datepicker_options' => array(
			'maxDate' => 0
		),
		'#disabled' => TRUE
	);
	$form['university'] = array(
		'#type' => 'item',
		'#markup' => $proposal_data->university,
		'#title' => t('University/Institute')
	);
	$form['country'] = array(
		'#type' => 'item',
		'#markup' => $proposal_data->country,
		'#title' => t('Country')
	);
	$form['all_state'] = array(
		'#type' => 'item',
		'#markup' => $proposal_data->state,
		'#title' => t('State')
	);
	$form['city'] = array(
		'#type' => 'item',
		'#markup' => $proposal_data->city,
		'#title' => t('City')
	);
	$form['pincode'] = array(
		'#type' => 'item',
		'#markup' => $proposal_data->pincode,
		'#title' => t('Pincode/Postal code')
	);
	$form['version'] = array(
		'#type' => 'item',
		'#title' => t('om version'),
		'#markup' => $proposal_data->version
	);
	$form['project_guide_name'] = array(
		'#type' => 'item',
		'#title' => t('Project guide'),
		'#markup' => $project_guide_name
	);
	$form['project_guide_email_id'] = array(
		'#type' => 'item',
		'#title' => t('Project guide email'),
		'#markup' => $project_guide_email_id
	);
	$form['project_title'] = array(
		'#type' => 'item',
		'#markup' => $proposal_data->project_title,
		'#title' => t('Title of the Flowsheet Project')
	);
	/*$form['dwsim_flowsheet_name'] = array(
		'#type' => 'item',
		'#title' => t('Name of the DWSIM flowsheet'),
		'#markup' => $proposal_data->dwsim_flowsheet_name,
	);*/
	$headers = array(
		"User defined compound",
		"CAS No."
	);
	$rows = array();
	$item = array(
		"{$proposal_data->process_development_compound_name}",
		"{$proposal_data->process_development_compound_cas_number}"
	);
	array_push($rows, $item);
	$prodata = theme('table', array(
		'header' => $headers,
		'rows' => $rows
	));
	$form['process_development_compound_name'] = array(
		'#type' => 'item',
		'#title' => t('Name of compound for which process development is carried out'),
		'#markup' => $prodata
	);
	$form['simulator_version_used'] = array(
		'#type' => 'item',
		'#title' => t('Simulator version used for creating the flowsheet'),
		'#markup' => $simulator_version_name,
	);
	$form['reference'] = array(
		'#type' => 'item',
		'#title' => t('Reference'),
		'#markup' => $proposal_data->reference,
	);
	/*
	$form['process_development_compound_cas_number'] = array(
	'#type' => 'item',
	'#title' => t('CAS Number of compound for which process development is carried out'),
	'#markup' => $proposal_data->process_development_compound_cas_number
	);*/
	$form['approval'] = array(
		'#type' => 'radios',
		'#title' => t('Select to approve/disapprove the proposal'),
		'#options' => array(
			'1' => 'Approve',
			'2' => 'Disapprove'
		),
		'#required' => TRUE
	);
	$form['message'] = array(
		'#type' => 'textarea',
		'#title' => t('Reason for disapproval'),
		'#attributes' => array(
			'placeholder' => t('Enter reason for disapproval in minimum 30 characters '),
			'cols' => 50,
			'rows' => 4
		),
		'#states' => array(
			'visible' => array(
				':input[name="approval"]' => array(
					'value' => '2'
				)
			)
		)
	);
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Submit')
	);
	$form['cancel'] = array(
		'#type' => 'item',
		'#markup' => l(t('Cancel'), 'chemical/flowsheeting-project/manage-proposal')
	);
	return $form;
}
function om_flowsheet_proposal_approval_form_validate($form, &$form_state)
{
	if ($form_state['values']['approval'] == 2)
	{
		if ($form_state['values']['message'] == '')
		{
			form_set_error('message', t('Reason for disapproval could not be empty'));
		} //$form_state['values']['message'] == ''
	} //$form_state['values']['approval'] == 2
}
function om_flowsheet_proposal_approval_form_submit($form, &$form_state)
{
	global $user;
	/* get current proposal */
	$proposal_id = (int) arg(4);
	// $proposal_q = db_query("SELECT * FROM {om_flowsheet_proposal} WHERE id = %d", $proposal_id);
	$query = db_select('om_flowsheet_proposal');
	$query->fields('om_flowsheet_proposal');
	$query->condition('id', $proposal_id);
	$proposal_q = $query->execute();
	if ($proposal_q)
	{
		if ($proposal_data = $proposal_q->fetchObject())
		{
			/* everything ok */
		} //$proposal_data = $proposal_q->fetchObject()
		else
		{
			drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
			drupal_goto('chemical/flowsheeting-project/manage-proposal');
			return;
		}
	} //$proposal_q
	else
	{
		drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
		drupal_goto('chemical/flowsheeting-project/manage-proposal');
		return;
	}
	if ($form_state['values']['approval'] == 1)
	{
		$query = "UPDATE {om_flowsheet_proposal} SET approver_uid = :uid, approval_date = :date, approval_status = 1 WHERE id = :proposal_id";
		$args = array(
			":uid" => $user->uid,
			":date" => time(),
			":proposal_id" => $proposal_id
		);
		db_query($query, $args);
		/* sending email */
		$user_data = user_load($proposal_data->uid);
		$email_to = $user_data->mail;
		$from = variable_get('om_flowsheet_from_email', '');
		$bcc = $user->mail . ', ' . variable_get('om_flowsheet_emails', '');
		$cc = variable_get('om_flowsheet_cc_emails', '');
		$params['om_flowsheet_proposal_approved']['proposal_id'] = $proposal_id;
		$params['om_flowsheet_proposal_approved']['user_id'] = $proposal_data->uid;
		$params['om_flowsheet_proposal_approved']['headers'] = array(
			'From' => $from,
			'MIME-Version' => '1.0',
			'Content-Type' => 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
			'Content-Transfer-Encoding' => '8Bit',
			'X-Mailer' => 'Drupal',
			'Cc' => $cc,
			'Bcc' => $bcc
		);
		if (!drupal_mail('om_flowsheet', 'om_flowsheet_proposal_approved', $email_to, language_default(), $params, $from, TRUE))
			drupal_set_message('Error sending email message.', 'error');
		drupal_set_message('om flowsheeting proposal No. ' . $proposal_id . ' approved. User has been notified of the approval.', 'status');
		drupal_goto('chemical/flowsheeting-project/manage-proposal');
		return;
	} //$form_state['values']['approval'] == 1
	else if ($form_state['values']['approval'] == 2)
	{
		$query = "UPDATE {om_flowsheet_proposal} SET approver_uid = :uid, approval_date = :date, approval_status = 2, dissapproval_reason = :dissapproval_reason WHERE id = :proposal_id";
		$args = array(
			":uid" => $user->uid,
			":date" => time(),
			":dissapproval_reason" => $form_state['values']['message'],
			":proposal_id" => $proposal_id
		);
		$result = db_query($query, $args);
		/* sending email */
		$user_data = user_load($proposal_data->uid);
		$email_to = $user_data->mail;
		$from = variable_get('om_flowsheet_from_email', '');
		$bcc = $user->mail . ', ' . variable_get('om_flowsheet_emails', '');
		$cc = variable_get('om_flowsheet_cc_emails', '');
		$params['om_flowsheet_proposal_disapproved']['proposal_id'] = $proposal_id;
		$params['om_flowsheet_proposal_disapproved']['user_id'] = $proposal_data->uid;
		$params['om_flowsheet_proposal_disapproved']['headers'] = array(
			'From' => $from,
			'MIME-Version' => '1.0',
			'Content-Type' => 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
			'Content-Transfer-Encoding' => '8Bit',
			'X-Mailer' => 'Drupal',
			'Cc' => $cc,
			'Bcc' => $bcc
		);
		if (!drupal_mail('om_flowsheet', 'om_flowsheet_proposal_disapproved', $email_to, language_default(), $params, $from, TRUE))
			drupal_set_message('Error sending email message.', 'error');
		drupal_set_message('om flowsheeting proposal No. ' . $proposal_id . ' dis-approved. User has been notified of the dis-approval.', 'error');
		drupal_goto('chemical/flowsheeting-project/manage-proposal');
		return;
	} //$form_state['values']['approval'] == 2
}
/******************************************************************************/
/*************************** PROPOSAL STATUS FORM *****************************/
/******************************************************************************/
function om_flowsheet_proposal_status_form($form, &$form_state)
{
	global $user;
	/* get current proposal */
	$proposal_id = (int) arg(4);
	//$proposal_q = db_query("SELECT * FROM {om_flowsheet_proposal} WHERE id = %d", $proposal_id);
	$query = db_select('om_flowsheet_proposal');
	$query->fields('om_flowsheet_proposal');
	$query->condition('id', $proposal_id);
	$proposal_q = $query->execute();
	if ($proposal_q)
	{
		if ($proposal_data = $proposal_q->fetchObject())
		{
			/* everything ok */
		} //$proposal_data = $proposal_q->fetchObject()
		else
		{
			drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
			drupal_goto('chemical/flowsheeting-project/manage-proposal');
			return;
		}
	} //$proposal_q
	else
	{
		drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
		drupal_goto('chemical/flowsheeting-project/manage-proposal');
		return;
	}
	$query = db_select('om_flowsheet_library');
	$query->fields('om_flowsheet_library');
	$query->condition('id', $proposal_data->simulator_version_id);
	$result = $query->execute()->fetchObject();
	$simulator_version_name = l($result->simulator_version_name, $result->link);
	$form['contributor_name'] = array(
		'#type' => 'item',
		'#markup' => l($proposal_data->name_title . ' ' . $proposal_data->contributor_name, 'user/' . $proposal_data->uid),
		'#size' => 250,
		'#title' => t('Student name')
	);
	$form['student_email_id'] = array(
		'#title' => t('Student Email'),
		'#type' => 'item',
		'#markup' => user_load($proposal_data->uid)->mail,
		'#title' => t('Email')
	);
	$form['month_year_of_degree'] = array(
		'#type' => 'date_popup',
		'#title' => t('Month and year of award of degree'),
		'#date_label_position' => '',
		'#description' => '',
		'#default_value' => $proposal_data->month_year_of_degree,
		'#date_format' => 'M-Y',
		'#date_increment' => 0,
		'#date_year_range' => '1960:+0',
		'#datepicker_options' => array(
			'maxDate' => 0
		),
		'#disabled' => TRUE
	);
	$form['university'] = array(
		'#type' => 'item',
		'#markup' => $proposal_data->university,
		'#title' => t('University/Institute')
	);
	$form['country'] = array(
		'#type' => 'item',
		'#markup' => $proposal_data->country,
		'#title' => t('Country')
	);
	$form['all_state'] = array(
		'#type' => 'item',
		'#markup' => $proposal_data->state,
		'#title' => t('State')
	);
	$form['city'] = array(
		'#type' => 'item',
		'#markup' => $proposal_data->city,
		'#title' => t('City')
	);
	$form['pincode'] = array(
		'#type' => 'item',
		'#markup' => $proposal_data->pincode,
		'#title' => t('Pincode/Postal code')
	);
	$form['version'] = array(
		'#type' => 'item',
		'#title' => t('om version'),
		'#markup' => $proposal_data->version
	);
	$form['project_guide_name'] = array(
		'#type' => 'item',
		'#title' => t('Project guide'),
		'#markup' => $proposal_data->project_guide_name
	);
	$form['project_guide_email_id'] = array(
		'#type' => 'item',
		'#title' => t('Project guide email'),
		'#markup' => $proposal_data->project_guide_email_id
	);
	$form['project_title'] = array(
		'#type' => 'item',
		'#markup' => $proposal_data->project_title,
		'#title' => t('Title of the Flowsheet Project')
	);
	/*$form['dwsim_flowsheet_name'] = array(
		'#type' => 'item',
		'#title' => t('Name of the DWSIM flowsheet'),
		'#markup' => $proposal_data->dwsim_flowsheet_name,
	);*/
	$form['process_development_compound_name'] = array(
		'#type' => 'item',
		'#title' => t('Name of compound for which process development is carried out'),
		'#markup' => $proposal_data->process_development_compound_name
	);
	$form['process_development_compound_cas_number'] = array(
		'#type' => 'item',
		'#title' => t('CAS Number of compound for which process development is carried out'),
		'#markup' => $proposal_data->process_development_compound_cas_number
	);
	$form['simulator_version_used'] = array(
		'#type' => 'item',
		'#title' => t('Simulator version used for creating the flowsheet'),
		'#markup' => $simulator_version_name,
	);
	$form['reference'] = array(
		'#type' => 'item',
		'#title' => t('Reference'),
		'#markup' => $proposal_data->reference
	);
	$proposal_status = '';
	switch ($proposal_data->approval_status)
	{
		case 0:
			$proposal_status = t('Pending');
			break;
		case 1:
			$proposal_status = t('Approved');
			break;
		case 2:
			$proposal_status = t('Dis-approved');
			break;
		case 3:
			$proposal_status = t('Completed');
			break;
		default:
			$proposal_status = t('Unkown');
			break;
	} //$proposal_data->approval_status
	
	$form['proposal_status'] = array(
		'#type' => 'item',
		'#markup' => $proposal_status,
		'#title' => t('Proposal Status')
	);
	if ($proposal_data->approval_status == 0)
	{
		$form['approve'] = array(
			'#type' => 'item',
			'#markup' => l('Click here', 'chemical/flowsheeting-project/manage-proposal/approve/' . $proposal_id),
			'#title' => t('Approve')
		);
	} //$proposal_data->approval_status == 0
	if ($proposal_data->approval_status == 1)
	{
		$form['completed'] = array(
			'#type' => 'checkbox',
			'#title' => t('Completed'),
			'#description' => t('Check if user has provided all the required files and pdfs.')
		);
	} //$proposal_data->approval_status == 1
	if ($proposal_data->approval_status == 2)
	{
		$form['message'] = array(
			'#type' => 'item',
			'#markup' => $proposal_data->message,
			'#title' => t('Reason for disapproval')
		);
	} //$proposal_data->approval_status == 2
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Submit')
	);
	$form['cancel'] = array(
		'#type' => 'markup',
		'#markup' => l(t('Cancel'), 'chemical/flowsheeting-project/manage-proposal/all')
	);
	return $form;
}
function om_flowsheet_proposal_status_form_submit($form, &$form_state)
{
	global $user;
	/* get current proposal */
	$proposal_id = (int) arg(4);
	//$proposal_q = db_query("SELECT * FROM {om_flowsheet_proposal} WHERE id = %d", $proposal_id);
	$query = db_select('om_flowsheet_proposal');
	$query->fields('om_flowsheet_proposal');
	$query->condition('id', $proposal_id);
	$proposal_q = $query->execute();
	if ($proposal_q)
	{
		if ($proposal_data = $proposal_q->fetchObject())
		{
			/* everything ok */
		} //$proposal_data = $proposal_q->fetchObject()
		else
		{
			drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
			drupal_goto('chemical/flowsheeting-project/manage-proposal');
			return;
		}
	} //$proposal_q
	else
	{
		drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
		drupal_goto('chemical/flowsheeting-project/manage-proposal');
		return;
	}
	/* set the book status to completed */
	if ($form_state['values']['completed'] == 1)
	{
		$up_query = "UPDATE om_flowsheet_proposal SET approval_status = :approval_status , actual_completion_date = :expected_completion_date WHERE id = :proposal_id";
		$args = array(
			":approval_status" => '3',
			":proposal_id" => $proposal_id,
			":expected_completion_date" => time()
		);
		$result = db_query($up_query, $args);
		CreateReadmeFileOpenModelicaFlowsheetingProject($proposal_id);
		if (!$result)
		{
			drupal_set_message('Error in update status', 'error');
			return;
		} //!$result
		/* sending email */
		$user_data = user_load($proposal_data->uid);
		$email_to = $user_data->mail;
		$from = variable_get('om_flowsheet_from_email', '');
		$bcc = $user->mail . ', ' . variable_get('om_flowsheet_emails', '');
		$cc = variable_get('om_flowsheet_cc_emails', '');
		$params['om_flowsheet_proposal_completed']['proposal_id'] = $proposal_id;
		$params['om_flowsheet_proposal_completed']['user_id'] = $proposal_data->uid;
		$params['om_flowsheet_proposal_completed']['headers'] = array(
			'From' => $from,
			'MIME-Version' => '1.0',
			'Content-Type' => 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
			'Content-Transfer-Encoding' => '8Bit',
			'X-Mailer' => 'Drupal',
			'Cc' => $cc,
			'Bcc' => $bcc
		);
		if (!drupal_mail('om_flowsheet', 'om_flowsheet_proposal_completed', $email_to, language_default(), $params, $from, TRUE))
			drupal_set_message('Error sending email message.', 'error');
		drupal_set_message('Congratulations! om flowsheeting proposal has been marked as completed. User has been notified of the completion.', 'status');
	} //$form_state['values']['completed'] == 1
	drupal_goto('chemical/flowsheeting-project/manage-proposal');
	return;
}
/******************************************************************************/
/**************************** PROPOSAL EDIT FORM ******************************/
/******************************************************************************/
function om_flowsheet_proposal_edit_form($form, &$form_state)
{
	global $user;
	/* get current proposal */
	$proposal_id = (int) arg(4);
	//$proposal_q = db_query("SELECT * FROM {om_flowsheet_proposal} WHERE id = %d", $proposal_id);
	$query = db_select('om_flowsheet_proposal');
	$query->fields('om_flowsheet_proposal');
	$query->condition('id', $proposal_id);
	$proposal_q = $query->execute();
	if ($proposal_q)
	{
		if ($proposal_data = $proposal_q->fetchObject())
		{
			/* everything ok */
		} //$proposal_data = $proposal_q->fetchObject()
		else
		{
			drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
			drupal_goto('chemical/flowsheeting-project/manage-proposal');
			return;
		}
	} //$proposal_q
	else
	{
		drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
		drupal_goto('chemical/flowsheeting-project/manage-proposal');
		return;
	}
	$query = db_select('om_flowsheet_library');
	$query->fields('om_flowsheet_library');
	$query->condition('id', $proposal_data->simulator_version_id);
	$result = $query->execute()->fetchObject();
	$simulator_version_name = $result->simulator_version_name;
	$user_data = user_load($proposal_data->uid);
	$form['name_title'] = array(
		'#type' => 'select',
		'#title' => t('Title'),
		'#options' => array(
			'Dr' => 'Dr',
			'Prof' => 'Prof',
			'Mr' => 'Mr',
			'Mrs' => 'Mrs',
			'Ms' => 'Ms'
		),
		'#required' => TRUE,
		'#default_value' => $proposal_data->name_title
	);
	$form['contributor_name'] = array(
		'#type' => 'textfield',
		'#title' => t('Name of the Proposer'),
		'#size' => 30,
		'#maxlength' => 250,
		'#required' => TRUE,
		'#default_value' => $proposal_data->contributor_name
	);
	$form['version'] = array(
		'#type' => 'select',
		'#title' => t('Version'),
		'#required' => TRUE,
		'#default_value' => $proposal_data->version,
		'#options' => _om_df_list_of_software_version()
	);
	$form['student_email_id'] = array(
		'#type' => 'item',
		'#title' => t('Email'),
		'#markup' => $user_data->mail
	);
	$form['contributor_contact_no'] = array(
		'#type' => 'textfield',
		'#title' => t('Contact No.'),
		'#size' => 10,
		'#default_value' => $proposal_data->contact_no
	);
	$form['month_year_of_degree'] = array(
		'#type' => 'date_popup',
		'#title' => t('Month and year of award of degree'),
		'#date_label_position' => '',
		'#description' => '',
		'#default_value' => $proposal_data->month_year_of_degree,
		'#date_format' => 'M-Y',
		'#date_increment' => 0,
		'#date_year_range' => '1960:+22',
	);
	$form['university'] = array(
		'#type' => 'textfield',
		'#title' => t('University/Institute'),
		'#size' => 80,
		'#maxlength' => 200,
		'#default_value' => $proposal_data->university
	);
	$form['project_guide_name'] = array(
		'#type' => 'textfield',
		'#title' => t('Project guide'),
		'#size' => 250,
		'#default_value' => $proposal_data->project_guide_name
	);
	$form['project_guide_email_id'] = array(
		'#type' => 'textfield',
		'#title' => t('Project guide email'),
		'#size' => 30,
		'#default_value' => $proposal_data->project_guide_email_id
	);
	$form['project_guide_university'] = array(
		'#type' => 'textfield',
		'#title' => t('Project Guide University/ Institute'),
		'#size' => 80,
		'#maxlength' => 200,
		'#default_value' => $proposal_data->project_guide_university
	);
	if($proposal_data->country == 'India'){
	$form['country'] = array(
		'#type' => 'select',
		'#title' => t('Country'),
		'#options' => array(
			'India' => 'India',
			'Others' => 'Others'
		),
		'#default_value' => $proposal_data->country,
		'#required' => TRUE,
		'#tree' => TRUE,
		'#validated' => TRUE
	);
	$form['all_state'] = array(
		'#type' => 'select',
		'#title' => t('State'),
		'#options' => _df_list_of_states(),
		'#default_value' => $proposal_data->state,
		'#validated' => TRUE,
		'#states' => array(
			'visible' => array(
				':input[name="country"]' => array(
					'value' => 'India'
				)
			)
		)
	);
	$form['city'] = array(
		'#type' => 'select',
		'#title' => t('City'),
		'#options' => _df_list_of_cities(),
		'#default_value' => $proposal_data->city,
		'#states' => array(
			'visible' => array(
				':input[name="country"]' => array(
					'value' => 'India'
				)
			)
		)
	);
}
else{ 
	$form['other_country'] = array(
		'#type' => 'textfield',
		'#title' => t('Country(Other than India)'),
		'#size' => 100,
		'#default_value' => $proposal_data->country,
		'#attributes' => array(
			'placeholder' => t('Enter your country name')
		),
		'#states' => array(
			'visible' => array(
				':input[name="country"]' => array(
					'value' => 'Others'
				)
			)
		)
	);
	$form['other_state'] = array(
		'#type' => 'textfield',
		'#title' => t('State(Other than India)'),
		'#size' => 100,
		'#attributes' => array(
			'placeholder' => t('Enter your state/region name')
		),
		'#default_value' => $proposal_data->state,
		'#states' => array(
			'visible' => array(
				':input[name="country"]' => array(
					'value' => 'Others'
				)
			)
		)
	);
	$form['other_city'] = array(
		'#type' => 'textfield',
		'#title' => t('City(Other than India)'),
		'#size' => 100,
		'#attributes' => array(
			'placeholder' => t('Enter your city name')
		),
		'#default_value' => $proposal_data->city,
		'#states' => array(
			'visible' => array(
				':input[name="country"]' => array(
					'value' => 'Others'
				)
			)
		)
	);
}
	$form['pincode'] = array(
		'#type' => 'textfield',
		'#title' => t('Pincode'),
		'#size' => 30,
		'#maxlength' => 6,
		'#default_value' => $proposal_data->pincode,
		'#attributes' => array(
			'placeholder' => 'Insert pincode of your city/ village....'
		)
	);
	$form['project_title'] = array(
		'#type' => 'textarea',
		'#title' => t('Title of the Flowsheet Project'),
		'#size' => 300,
		'#maxlength' => 350,
		'#required' => TRUE,
		'#default_value' => $proposal_data->project_title
	);
	/*$form['dwsim_flowsheet_name'] = array(
            '#type' => 'textfield',
            '#title' => t('The name of DWSIM flowsheet along with its serial number.'),
            '#default_value' => $proposal_data->dwsim_flowsheet_name,
            '#validated' => TRUE,
	);*/
	$form['reference'] = array(
		'#type' => 'textarea',
		'#title' => t('Reference of the Flowsheet Project'),
		'#size' => 250,
		'#maxlength' => 250,
		'#default_value' => $proposal_data->reference
	);
	$form['process_development_compound_name'] = array(
		'#type' => 'textfield',
		'#title' => t('Name of compound for which process development is carried out'),
		'#size' => 50,
		'#default_value' => $proposal_data->process_development_compound_name 
	);
	$form['process_development_compound_cas_no'] = array(
		'#type' => 'textfield',
		'#title' => t('CAS number for compound which process development is carried out'),
		'#size' => 50,
		'#default_value' => $proposal_data->process_development_compound_cas_number
		
	);
	$form['simulator_version_used'] = array(
			'#type' => 'select',
			'#title' => t('Simulator version used for creating the flowsheet'),
			'#options' => _df_list_of_simulator_version_used(),
			'#required' => TRUE,
			'#default_value' => $simulator_version_name,
		);
	$form['delete_proposal'] = array(
		'#type' => 'checkbox',
		'#title' => t('Delete Proposal')
	);
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Submit')
	);
	$form['cancel'] = array(
		'#type' => 'item',
		'#markup' => l(t('Cancel'), 'chemical/flowsheeting-project/manage-proposal')
	);
	return $form;
}
/*
function om_flowsheet_proposal_edit_form_validate($form, &$form_state)
{
$proposal_id = (int) arg(3);
// check before delete proposal 
if ($form_state['values']['delete_proposal'] == 1)
{
//$experiment_q = db_query("SELECT * FROM {om_flowsheet_experiment} WHERE proposal_id = %d", $proposal_id);
$query = db_select('om_flowsheet_user_defined_compound');
$query->fields('om_flowsheet_user_defined_compound');
$query->condition('proposal_id', $proposal_id);
$experiment_q = $query->execute();
while ($experiment_data = $experiment_q->fetchObject())
{
//$solution_q = db_query("SELECT * FROM {om_flowsheet_solution} WHERE experiment_id = %d", $experiment_data->id);
$query = db_select('om_flowsheet_user_defined_compound');
$query->fields('om_flowsheet_user_defined_compound');
$query->condition('proposal_id', $proposal_id);
$solution_q = $query->execute();
if ($solution_q->fetchObject())
{
form_set_error('', t('Cannot delete proposal since there are solutions already uploaded. Use the "Bulk Manage" interface to delete this proposal'));
} //$solution_q->fetchObject()
} //$experiment_data = $experiment_q->fetchObject()
} //$form_state['values']['delete_proposal'] == 1
return;
}*/
function om_flowsheet_proposal_edit_form_submit($form, &$form_state)
{
	global $user;
	/* get current proposal */
	$proposal_id = (int) arg(4);
	// $proposal_q = db_query("SELECT * FROM {om_flowsheet_proposal} WHERE id = %d", $proposal_id);
	$query = db_select('om_flowsheet_proposal');
	$query->fields('om_flowsheet_proposal');
	$query->condition('id', $proposal_id);
	$proposal_q = $query->execute();
	if ($proposal_q)
	{
		if ($proposal_data = $proposal_q->fetchObject())
		{
			/* everything ok */
		} //$proposal_data = $proposal_q->fetchObject()
		else
		{
			drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
			drupal_goto('chemical/flowsheeting-project/manage-proposal');
			return;
		}
	} //$proposal_q
	else
	{
		drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
		drupal_goto('chemical/flowsheeting-project/manage-proposal');
		return;
	}
	/* delete proposal */
	if ($form_state['values']['delete_proposal'] == 1)
	{
		/* sending email */
		$user_data = user_load($proposal_data->uid);
		$email_to = $user_data->mail;
		$from = variable_get('om_flowsheet_from_email', '');
		$bcc = variable_get('om_flowsheet_emails', '');
		$cc = variable_get('om_flowsheet_cc_emails', '');
		$params['om_flowsheet_proposal_deleted']['proposal_id'] = $proposal_id;
		$params['om_flowsheet_proposal_deleted']['user_id'] = $proposal_data->uid;
		//$params['om_flowsheet_proposal_deleted']['file_name'] = $_FILES['files']['name'][$file_form_name];
		$params['om_flowsheet_proposal_deleted']['headers'] = array(
			'From' => $from,
			'MIME-Version' => '1.0',
			'Content-Type' => 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
			'Content-Transfer-Encoding' => '8Bit',
			'X-Mailer' => 'Drupal',
			'Cc' => $cc,
			'Bcc' => $bcc
		);
		if (!drupal_mail('om_flowsheet', 'om_flowsheet_proposal_deleted', $email_to, user_preferred_language($user), $params, $from, TRUE))
			drupal_set_message('Error sending email message.', 'error');
		drupal_set_message(t('om Flowsheeting proposal has been deleted.'), 'status');
		//db_query("DELETE FROM {om_flowsheet_proposal} WHERE id = %d", $proposal_id);
		if (om_rrmdir_project($proposal_id) == TRUE)
		{
			db_delete('om_flowsheet_submitted_abstracts_file')->condition('proposal_id', $proposal_id)->execute();
			db_delete('om_flowsheet_submitted_abstracts')->condition('proposal_id', $proposal_id)->execute();
			$query = db_delete('om_flowsheet_proposal');
			$query->condition('id', $proposal_id);
			$num_deleted = $query->execute();
			drupal_set_message(t('Proposal Deleted'), 'status');
			drupal_goto('chemical/flowsheeting-project/manage-proposal');
			return;
		} //om_rrmdir_project($proposal_id) == TRUE
	} //$form_state['values']['delete_proposal'] == 1
	/* update proposal */
	$v = $form_state['values'];
	$simulator_version_used = $v['simulator_version_used'];
	$query = db_select('om_flowsheet_library');
	$query->fields('om_flowsheet_library');
	$query->condition('simulator_version_name', $simulator_version_used);
	$result = $query->execute()->fetchObject();
	$simulator_version_used_id = $result->id;
	$project_title = $v['project_title'];
	$proposar_name = $v['name_title'] . ' ' . $v['contributor_name'];
	$university = $v['university'];
	$directory_names = _om_df_dir_name($project_title, $proposar_name);
	if (om_DF_RenameDir($proposal_id, $directory_names))
	{
		$directory_name = $directory_names;
	} //LM_RenameDir($proposal_id, $directory_names)
	else
	{
		return;
	}
	$query = "UPDATE om_flowsheet_proposal SET 
				name_title=:name_title,
				contributor_name=:contributor_name,
				version = :version,
				university=:university,
				city=:city,
				pincode=:pincode,
				state=:state,
				project_title=:project_title,
				reference=:reference,
				directory_name=:directory_name,
				project_guide_university=:project_guide_university,
				project_guide_email_id=:project_guide_email_id,
				project_guide_name=:project_guide_name,
				month_year_of_degree=:month_year_of_degree,
				process_development_compound_name=:process_development_compound_name,
				process_development_compound_cas_number=:process_development_compound_cas_number,
				simulator_version_id = :simulator_version_id
				WHERE id=:proposal_id";
	$args = array(
		':name_title' => $v['name_title'],
		':contributor_name' => $v['contributor_name'],
		':version' => $v['version'],
		':university' => $v['university'],
		':city' => $v['city'],
		':pincode' => $v['pincode'],
		':state' => $v['all_state'],
		':project_title' => $project_title,
		':reference' => $v['reference'],
		':directory_name' => $directory_name,
		':project_guide_university' => $v['project_guide_university'],
		':project_guide_email_id' => $v['project_guide_email_id'],
		':project_guide_name' => $v['project_guide_name'],
		':month_year_of_degree' => $v['month_year_of_degree'],
		':process_development_compound_name' => $v['process_development_compound_name'],
		':process_development_compound_cas_number' => $v['process_development_compound_cas_no'],
		':simulator_version_id' => $simulator_version_used_id,
		':proposal_id' => $proposal_id
	);
	$result = db_query($query, $args);
	drupal_set_message(t('Proposal Updated'), 'status');
}
