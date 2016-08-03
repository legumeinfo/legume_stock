<?php
$options = array('return_array' => 1);

$stock = $node->stock;
$organism = $stock->organism_id;

$stock = chado_expand_var($stock, 'table', 'stockprop', $options);
$props = array();
if (isset($stock->stockprop)) {
  $properties = $stock->stockprop;
  foreach ($properties as $property){
    $props[$property->type_id->name] = $property->value;
  }
}
//echo "<pre>";var_dump($props);echo "</pre>";

// expand the text fields
$stock = chado_expand_var($stock, 'field', 'stock.description');
$stock = chado_expand_var($stock, 'field', 'stock.uniquename'); ?>

<div class="tripal_stock-data-block-desc tripal-data-block-desc"></div> <?php  

// the $headers array is an array of fields to use as the colum headers. 
// additional documentation can be found here 
// https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
// This table for the stock has a vertical header (down the first column)
// so we do not provide headers here, but specify them in the $rows array below.
$headers = array();

// the $rows array contains an array of rows where each row is an array
// of values for each column of the table in that row.  Additional documentation
// can be found here:
// https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7 
$rows = array();

/////////////
// Stock Unique Name
$rows[] = array(
  array(
    'data'   => 'Stock Name',
    'header' => TRUE,
    'width'  => 200,
  ),
  $stock->uniquename
);

/////////////
// Stock Type
$rows[] = array(
  array(
    'data' => 'Stock type',
    'header' => TRUE,
  ),
  ucwords(preg_replace('/_/', ' ', $stock->type_id->name))
);

/////////////
// Organism
$organism = $stock->organism_id->genus ." " . $stock->organism_id->species ." (" . $stock->organism_id->common_name .")";
if (property_exists($stock->organism_id, 'nid')) {
  $organism = l("<i>" . $stock->organism_id->genus . " " . $stock->organism_id->species . "</i> (" . $stock->organism_id->common_name .")", "node/".$stock->organism_id->nid, array('html' => TRUE));
}
$rows[] = array(
  array(
    'data' => 'Organism',
    'header' => TRUE
  ),
  $organism
);

/////////////
// Get cross-references

// via stock.dbxref_id
$references = array();
if ($stock->dbxref_id) {
  $stock->dbxref_id->is_primary = 1;  
  if ($stock->dbxref_id->db_id->name == 'GRIN') {
    // important special case
    $grin_id = $stock->dbxref_id->accession; // NOT the PI number: internal ID
    if ($stock->type_id->name == 'accession') {
      $grin_accession = $stock->uniquename;
    }
    else {
      $grin_accession = (isset($props['grin_accession'])) 
                      ? $props['grin_accession'] : 'unknown';
    }
//echo "<pre>";var_dump($stock->type_id);echo"</pre>";
    $grin_url = $stock->dbxref_id->db_id->urlprefix . $grin_id;
  }
  else {
    // just add to reference list
    $references[] = $stock->dbxref_id;
  }
}

// via stock_dbxref
$stock = chado_expand_var($stock, 'table', 'stock_dbxref', $options);
$stock_dbxrefs = $stock->stock_dbxref;
if (count($stock_dbxrefs) > 0 ) {
  foreach ($stock_dbxrefs as $stock_dbxref) {    
    if($stock_dbxref->dbxref_id->db_id->name == 'GFF_source'){
      // check to see if the reference 'GFF_source' is there.  This reference is
      // used to see if the Chado Perl GFF loader was used to load the stocks   
    }
    else {
      $references[] = $stock_dbxref->dbxref_id;
    }
  }
}

// Show GRIN link, if any
if ($grin_id && $grin_url) {
  $link = l($grin_accession, $grin_url, array('attributes' => array('target' => '_blank')));
  $rows[] = array(
    array(
      'data' => 'GRIN Global accession',
      'header' => TRUE
    ),
    $link
  );
}

// Show origin, if known
if (isset($props['origin'])) {
  $rows[] = array(
    array(
      'data' => 'Origin',
      'header' => TRUE
    ),
    $props['origin']
  );
}

// Link to GIS map
//TODO: do this as dbxref
if ($grin_accession) {
  # URL looks like /germplasm/map#?accessionIds=PI%20295267&showAccessionDetail
  $gis_url = "/germplasm/map";
  $link_ops = array('query' => 
                array(
                  'accessionIds'        =>"$grin_accession",
                  'showAccessionDetail' => '',
                )
  );
  $link = l('GIS', $gis_url, $link_ops);
  // ugly: need to add a # here because l() munges it
  $link = str_replace('?', '#?', $link);
  // also ugly: need to remove trailing =
  $link = str_replace('showAccessionDetail=', 'showAccessionDetail', $link);
  $rows[] = array(
    array(
      'data' => 'Geographic location',
      'header' => TRUE,
    ),
    $link,
  );
}


// Show image link, if any
$stock = chado_expand_var($stock, 'table', 'stock_eimage');
if (isset($stock->stock_eimage)) {
  $eimage = $stock->stock_eimage;
  if (!is_array($eimage)) {
    $eimage = array($eimage);
  }
  $html .= '';
  foreach ($eimage as $image) {
    $image_name = $image->eimage_id->eimage_data;
    $image_type = $image->eimage_id->eimage_type;
    $image_url = $image->eimage_id->image_uri;
    $contents = "<img src='$image_url' height=100px class='legume_image'><br>$image_type";
    $html .= "<div class='legume_image'><a href='$image_url'>$contents</a></div>";
  }
  
  $rows[] = array(
    array(
      'data' => 'Image',
      'header' => TRUE
    ),
    $html
  );
}


// if a mapping population, show parents and link to map
if ($stock->type_id->name == 'Mapping Population') {
  $stock = loadMappingData($stock);
  
  $parents = '';
  $url = '/node/';
  if ($stock->parent1) {
    $parents .= l($stock->parent1->name, $url.$stock->parent1->nid) . ' ';
  }
  if ($stock->parent2) {
    $parents .= l($stock->parent2->name, $url.$stock->parent2->nid);
  }
  $rows[] = array(
    array('data' => 'Parents',
          'header' => TRUE,
    ),
    $parents,
  );
  
  $map = '';
  if ($stock->map) {
    $map = l($stock->map->name, $url.$stock->map->nid);
    $map .= "<br>" . $stock->map->description;
    $rows[] = array(
      array('data' => 'Map',
            'header' => TRUE,
      ),
      $map,
    );
  }
}//is mapping population


/////////////
// allow site admins to see the stock ID
if (user_access('view ids')) {
  // stock ID
  $rows[] = array(
    array(
      'data' => 'Stock ID',
      'header' => TRUE,
      'class' => 'tripal-site-admin-only-table-row',
    ),
    array(
      'data' => $stock->stock_id,
      'class' => 'tripal-site-admin-only-table-row',
    ),
  );
}
// Is Obsolete Row
if($stock->is_obsolete == TRUE){
  $rows[] = array(
    array(
      'data' => '<div class="tripal_stock-obsolete">This stock is obsolete</div>',
      'colspan' => 2
    ),
  );
}
// the $table array contains the headers and rows array as well as other
// options for controlling the display of the table.  Additional
// documentation can be found here:
// https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
$table = array(
  'header' => $headers,
  'rows' => $rows,
  'attributes' => array(
    'id' => 'tripal_stock-table-base',
    'class' => 'tripal-data-table'
  ),
  'sticky' => FALSE,
  'caption' => '',
  'colgroups' => array(),
  'empty' => '',
);

// once we have our table array structure defined, we call Drupal's theme_table()
// function to generate the table.
print theme_table($table);

// add in the description if there is one
if (property_exists($stock, 'description')) { ?>
  <div style="text-align: justify"><?php print $stock->description; ?></div> <?php  
} 