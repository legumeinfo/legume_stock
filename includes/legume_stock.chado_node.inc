<?php
/**
 * @file
 * Implementation of hooks to create a stock content type
 */



/**
 * Implements hook_node_view().
 * Acts on all content types.
 *
 * @ingroup legume_stock
 */
function legume_stock_node_view($node, $view_mode, $langcode) {
  if ($node->type != 'chado_stock') { return; }

  $path = drupal_get_path('module', 'legume_stock');
  
  switch ($view_mode) {
    case 'full':
      // we want to override the base stock theme
      $node->content['tripal_stock_base'] = array(
          '#markup' => theme("tripal_legume_stock_base",
                             array('node' => $node)),
          '#tripal_toc_id'    => 'base',
          '#tripal_toc_title' => 'Stock Overview',
          '#weight' => -100,
        );
      break;
      
    case 'teaser':
      break;
      
    default:
      break;
  }
}


function loadMappingData($stock) {
  // Get parent1
  $sql = "
    SELECT s.*, cs.nid FROM {stock_relationship} rs
      INNER JOIN {stock} s ON s.stock_id=rs.subject_id
      LEFT JOIN chado_stock cs ON cs.stock_id=rs.subject_id
    WHERE object_id=" . $stock->stock_id . "
          AND rs.type_id=(SELECT cvterm_id FROM cvterm 
                          WHERE name='Parent1' 
                                AND cv_id=(SELECT cv_id FROM cv 
                                           WHERE name='stock_relationship'))";
  $result = chado_query($sql);
  foreach ($result as $r) {
    $stock->parent1 = $r;
    break; //only one row
  }
  
  // Get parent2
  $sql = "
    SELECT s.*, cs.nid FROM {stock_relationship} rs
      INNER JOIN {stock} s ON s.stock_id=rs.subject_id
      LEFT JOIN chado_stock cs ON cs.stock_id=rs.subject_id
    WHERE object_id=" . $stock->stock_id . "
          AND rs.type_id=(SELECT cvterm_id FROM cvterm 
                          WHERE name='Parent2' 
                                AND cv_id=(SELECT cv_id FROM cv 
                                           WHERE name='stock_relationship'))";
  $result = chado_query($sql);
  foreach ($result as $r) {
    $stock->parent2 = $r;
    break; //only one row
  }
  
  // Get map
  $sql = "
    SELECT f.*, cf.nid FROM {featuremap} f
      INNER JOIN {featuremap_stock} fs ON fs.featuremap_id=f.featuremap_id
      INNER JOIN chado_featuremap cf ON cf.featuremap_id=f.featuremap_id
    WHERE fs.stock_id = " . $stock->stock_id;
  $result = chado_query($sql);
  foreach ($result as $r) {
    $stock->map = $r;
    break; //only one row
  }

  return $stock;
}


function loadTraitData($stock) {
  $traits = array();
  $sql = "
    SELECT s.stock_id, s.name AS stock, a.definition AS attr, 
           m.definition AS method, ph.value, v.definition AS cvalue
    FROM phenotype ph
      INNER JOIN stock_phenotype sp ON sp.phenotype_id=ph.phenotype_id
      INNER JOIN stock s ON s.stock_id=sp.stock_id
      INNER JOIN cvterm a ON a.cvterm_id=ph.attr_id
      INNER JOIN cvterm m ON m.cvterm_id=ph.assay_id
      LEFT OUTER JOIN cvterm v ON v.cvterm_id=cvalue_id
    WHERE s.stock_id=" . $stock->stock_id . "
    ORDER BY m.definition, a.definition";
  $result = chado_query($sql);
  foreach ($result as $r) {
    $traits[] = $r;
  }//each trait
  
  return $traits;
}


function getCountryName($code) {
  $sql = "
    SELECT definition FROM cvterm 
    WHERE name='$code'
          AND cv_id=(SELECT cv_id FROM cv WHERE name='GRIN_countries')";
  $result = chado_query($sql);
  foreach ($result as $r) {
    // only one result
    return $r->definition;
  }
  
  return false;
}