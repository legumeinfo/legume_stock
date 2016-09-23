<?php
$options = array('return_array' => 1);

$stock = $node->stock;
$organism = $stock->organism_id;

$stock = chado_expand_var($stock, 'table', 'stockprop', $options);
$props = array();
$synonyms = array();
if (isset($stock->stockprop)) {
  $properties = $stock->stockprop;
  foreach ($properties as $property){
    if ($property->type_id->name == 'alias') {
      $synonyms[] = $property->value;
    }
    else {
      $props[$property->type_id->name] = $property->value;
    }
  }
}
//echo "<pre>";var_dump($props);echo "</pre>";
//echo "<pre>";var_dump($synonyms);echo "</pre>";

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

// Stock Synonyms
if ($synonyms && count($synonyms)) {
  $rows[] = array(
    array(
      'data'   => 'Other Name(s)',
      'header' => TRUE,
      'width'  => 200,
    ),
    implode(', ', $synonyms),
  );
}

/////////////
// Stock Type
$rows[] = array(
  array(
    'data' => 'Stock type',
    'header' => TRUE,
  ),
  ucwords(preg_replace('/_/', ' ', $stock->type_id->name))
);

///////////////////
// Crop/Market Type
$market_type = ($props['crop']) ? $props['crop'] : 'unspecified';
$rows[] = array(
  array(
    'data' => 'Market type',
    'header' => TRUE,
  ),
  $market_type
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
    if ($stock->type_id->name == 'Accession') {
      $grin_accession = $stock->uniquename;
    }
    else {
      $grin_accession = (isset($props['grin_accession'])) 
                      ? $props['grin_accession'] : false;
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
/* 09/06/16 data incorrect; needs to be repaired in spreadsheet and reloaded
if (isset($props['origin'])) {
  $rows[] = array(
    array(
      'data' => 'Origin',
      'header' => TRUE
    ),
    $props['origin']
  );
}
*/

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

// Description
if (property_exists($stock, 'description')) { 
  $rows[] = array(
    array('data' => 'Description',
          'header' => TRUE,
    ),
    $stock->description,
  );
}

/////// SEPARATOR /////////

$rows[] = array(
  array(
    'data' => '',
    'header' => TRUE,
    'height' => 6,
    'style' => 'background-color:white',
  ),
  array(
    'data' => '',
    'style' => 'background-color:white',
  ),
);

/////// TRAITS SECTION /////////
$traits = loadTraitData($stock);
$rows[] = array(
  array(
    'data' => 'Traits',
    'header' => TRUE,
    'colspan' => 2,
    'style' => 'background-color:#c9c9c9;color:#101010',
  ),
);

$methods = array();
foreach ($traits as $trait) {
  if (!isset($methods[$trait->method])) {
    $methods[$trait->method] = array();
  }
  $value = ($trait->value) ? $trait->value : $trait->cvalue;
  $attr  = $trait->attr;
  $methods[$trait->method][] = array(
    'attr'  => $attr,
    'value' => $value,
  );
}

foreach (array_keys($methods) as $method) {
  $trait_rows = array();
  foreach ($methods[$method] as $trait) {
    $trait_rows[] = array(
      $trait['attr'],
      $trait['value'],
    );
  }
  $trait_table = array(
    'header' => array('trait', 'value'),
    'rows' => $trait_rows,
    'attributes' => array(
      'id' => 'tripal_stock-table-base',
      'class' => 'tripal-data-table legume_trait_table'
    ),
    'sticky' => FALSE,
    'caption' => '',
    'colgroups' => array(),
    'empty' => '',
  );
  
  $rows[] = array(
    array('data' => $method,
          'header' => TRUE,
    ),
    theme_table($trait_table),
  );
}

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

