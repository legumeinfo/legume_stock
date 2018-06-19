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
    SELECT p.name AS study, s.stock_id, s.name AS stock, t.name AS trait,
           m.name AS methodname, m.definition AS method_desc,
           sc.name AS scalename, ph.value AS literal_value, 
           SPLIT_PART(c.name, '|', 1) AS code_value, 
           c.definition AS code_description
    FROM {phenotype ph 
      INNER JOIN {project_phenotype} pph ON pph.phenotype_id=ph.phenotype_id
      INNER JOIN {project} p ON p.project_id=pph.project_id
      INNER JOIN {stock_phenotype} sph ON sph.phenotype_id=ph.phenotype_id
      INNER JOIN {stock} s ON s.stock_id=sph.stock_id
      INNER JOIN {cvterm} t ON t.cvterm_id=ph.attr_id
      LEFT JOIN {cvterm} m ON m.cvterm_id=ph.assay_id
      LEFT JOIN {cvterm_relationship} sr ON sr.object_id=m.cvterm_id
        AND sr.type_id=(SELECT cvterm_id FROM {cvterm} WHERE name='scale_of')
      LEFT JOIN {cvterm} sc ON sc.cvterm_id=sr.subject_id
      LEFT JOIN {cvterm} c ON c.cvterm_id=ph.cvalue_id
    WHERE s.stock_id=" . $stock->stock_id . "
    ORDER BY m.name, t.name";
//drupal_set_message(preg_replace("/[\{\}]/", '', $sql));
  $result = chado_query($sql);
  foreach ($result as $r) {
    $traits[] = array(
      'study'       => $r->study, 
      'stock_id'    => $r->stock_id, 
      'stock'       => $r->stock, 
      'trait'       => $r->trait,
      'method'      => $r->methodname,
      'method_desc' => $r->method_desc,
      'scale'       => $r->scalename, 
      'value'       => $r->literal_value,
      'cvalue'      => $r->code_value,
      'cvalue_desc' => $r->code_description,
    );
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