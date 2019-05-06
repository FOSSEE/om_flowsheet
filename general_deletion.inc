<?php
/******************************************************************************/
/****************************** DELETION FUNCTIONS ****************************/
/******************************************************************************/
function om_flowsheet_delete_abstract_file($abstract_id)
{
	$status = TRUE;
	$root_path = om_flowsheet_path();
	$abstract_q = db_query("SELECT * from om_flowsheet_proposal dfp
LEFT JOIN om_flowsheet_submitted_abstracts dfsa ON dfp.id = dfsa.proposal_id
WHERE dfsa.id = :abstract_id", array(
		":abstract_id" => $abstract_id
	));
	$abstract_data = $abstract_q->fetchObject();
	if (!$abstract_data)
	{
		drupal_set_message('Invalid flowsheet project abstract.', 'error');
		return FALSE;
	} //!$abstract_data
	/* deleting solutions */
	$query = db_select('om_flowsheet_submitted_abstracts_file');
	$query->fields('om_flowsheet_submitted_abstracts_file');
	$query->condition('submitted_abstract_id', $abstract_id);
	$abstract_f_q = $query->execute();
	
	while ($abstract_f_data = $abstract_f_q->fetchObject())
	{
		unlink($root_path . $abstract_data->directory_name . '/' . $abstract_f_data->filepath);
		db_delete('om_flowsheet_submitted_abstracts_file')->condition('id', $abstract_f_data->id)->execute();
	} 
	

	return True;
}
function om_flowsheet_abstract_delete_project($proposal_id)
{
	$status = TRUE;
	$root_path = om_flowsheet_path();
	//$proposal_q = db_query("SELECT * FROM {om_flowsheet_proposal} WHERE id = %d", $lab_id);
	$query = db_select('om_flowsheet_proposal');
	$query->fields('om_flowsheet_proposal');
	$query->condition('id', $proposal_id);
	$proposal_q = $query->execute();
	$proposal_data = $proposal_q->fetchObject();
	if (!$proposal_data)
	{
		drupal_set_message('Invalid Flowsheeting Project.', 'error');
		return FALSE;
	} //!$proposal_data
	$query = db_select('om_flowsheet_submitted_abstracts');
	$query->fields('om_flowsheet_submitted_abstracts');
	$query->condition('proposal_id', $proposal_data->id);
	$abstract_q = $query->execute();
	while ($abstract_data = $abstract_q->fetchObject())
	{
		if (!om_flowsheet_delete_abstract_file($abstract_data->id))
		{
			$status = FALSE;
		} //!om_flowsheet_delete_abstract_file($abstract_data->id)
	} //$abstract_data = $abstract_q->fetchObject()
	
	$dir_main_path = $root_path . $proposal_data->directory_name;
	//var_dump($dir_main_path);die;
	if (is_dir($dir_main_path))
	{
		$res_main = rmdir($dir_main_path);
	} //is_dir($dir_main_path)
	
	db_delete('om_flowsheet_submitted_abstracts')->condition('proposal_id', $proposal_data->id)->execute();
	db_delete('om_flowsheet_proposal')->condition('id', $proposal_data->id)->execute();
	return $status;
}