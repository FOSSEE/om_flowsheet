<?php
// om Flowsheet display completed proposals
function om_flowsheet_completed_proposals_all()
{
	$output = "";
	$query = db_select('om_flowsheet_proposal');
	$query->fields('om_flowsheet_proposal');
	$query->condition('approval_status', 3);
	$query->orderBy('actual_completion_date', 'ASC');
	//$query->condition('is_completed', 1);
	$result = $query->execute();
	if ($result->rowCount() == 0)
	{
		$output .= "Work has been completed for the following flow sheets. We welcome your contributions. For more details, please visit ".l("https://om.fossee.in/flowsheeting-project","https://om.fossee.in/flowsheeting-project", array('attributes' => array('target' => '_blank')))."<br>"."<h4>"."If you are looking for flowsheeting project ideas, ".l("click here","https://om.fossee.in/flowsheeting-ideas",array('attributes' => array('class' => array('flash_content'), 'target' => '_blank')))."</h4>"."<hr>";

	} //$result->rowCount() == 0
	else
	{
		$output .= "Work has been completed for the following flow sheets. We welcome your contributions. For more details, please visit ".l("https://om.fossee.in/flowsheeting-project","https://om.fossee.in/flowsheeting-project", array('attributes' => array('target' => '_blank')))."<br>"."<h4>"."If you are looking for flowsheeting project ideas, ".l("click here","https://om.fossee.in/flowsheeting-ideas",array('attributes' => array('class' => array('flash_content'), 'target' => '_blank')))."</h4>"."<hr>";
		$preference_rows = array();
		$i = 1;
		while ($row = $result->fetchObject())
		{
			$completion_date = date("Y", $row->actual_completion_date);
			$preference_rows[] = array(
				$i,
				l($row->project_title, "flowsheeting-project/om-flowsheet-run/" . $row->id, array('attributes' => array('title' => 'This is a zip file containing a pdf (abstract) and a dwxml/dwxmz file which is the om flow sheet which is to be viewed by right clicking on the file and opening with om.'))),
				$row->contributor_name,
				$row->university,
				$completion_date
			);
			$i++;
		} //$row = $result->fetchObject()
		$preference_header = array(
			'No',
			'Flowsheet Project',
			'Contributor Name',
			'Institution',
			'Year'
		);
		$output .= theme('table', array(
			'header' => $preference_header,
			'rows' => $preference_rows
		));
	}
	return $output;
}
// om Flowsheet display in progress proposals
function om_flowsheet_progress_all()
{
	$page_content = "";
	$query = db_select('om_flowsheet_proposal');
	$query->fields('om_flowsheet_proposal');
	$query->condition('approval_status', 1);
	$query->condition('is_completed', 0);
	$result = $query->execute();
	if ($result->rowCount() == 0)
	{
		$page_content .= "Work is in progress for the following flowsheets under Flowsheeting Project<hr>";
	} //$result->rowCount() == 0
	else
	{
		$page_content .= "Work is in progress for the following flowsheets under Flowsheeting Project<hr>";;
		$preference_rows = array();
		$i = 1;
		while ($row = $result->fetchObject())
		{
			$approval_date = date("Y", $row->approval_date);
			$preference_rows[] = array(
				$i,
				$row->project_title,
				$row->contributor_name,
				$row->university,
				$approval_date
			);
			$i++;
		} //$row = $result->fetchObject()
		$preference_header = array(
			'No',
			'Flowsheet Project',
			'Contributor Name',
			'Institution',
			'Year'
		);
		$page_content .= theme('table', array(
			'header' => $preference_header,
			'rows' => $preference_rows
		));
	}
	return $page_content;
}