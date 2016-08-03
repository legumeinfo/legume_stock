<?php
/**
 * @file
 * Implementation of hooks to create a stock content type
 */



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