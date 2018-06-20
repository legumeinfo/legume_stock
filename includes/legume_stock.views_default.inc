<?php
/**
 * @file
 * Describe default stock views
 */

/**
 * Implements hook_views_default_views().
 *
 * @ingroup tripal_gene
 */
function legume_stock_views_default_views() {
  $views = array();

  $view = legume_stock_defaultvalue_stock();
  $views[$view->name] = $view;

  return $views;
}//legume_stock_views_default_views


/**
 * Defines the gene user search default view.
 *
 * @ingroup tripal_gene
 */
function legume_stock_defaultvalue_stock() {
  $view = new view();
  $view->name = 'stock_search';
  $view->description = 'Stock search View';
  $view->tag = 'default';
  $view->base_table = 'stock_search';
  $view->human_name = 'Stock Search';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; 

  // Display: Master
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Stock Search';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'none';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '10';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'stockname' => 'stockname',
    'common_name' => 'common_name',
    'collection' => 'collection',
  );
  
  $handler->display->display_options['style_options']['default'] = 'stockname';
  
  $handler->display->display_options['style_options']['info'] = array(
    'stockname' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'common_name' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'collection' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  
  // Footer: Global: Result summary
  $handler->display->display_options['footer']['result']['id'] = 'result';
  $handler->display->display_options['footer']['result']['table'] = 'views';
  $handler->display->display_options['footer']['result']['field'] = 'result';
  $handler->display->display_options['footer']['result']['label'] = 'results';
  $handler->display->display_options['footer']['result']['content'] = 'Displaying @start - @end of @total stocks';
  
  // No results behavior: Global: Text area
  $handler->display->display_options['empty']['area']['id'] = 'area';
  $handler->display->display_options['empty']['area']['table'] = 'views';
  $handler->display->display_options['empty']['area']['field'] = 'area';
  $handler->display->display_options['empty']['area']['label'] = 'No results';
  $handler->display->display_options['empty']['area']['empty'] = TRUE;
  $handler->display->display_options['empty']['area']['content'] = 'No results found. ';
  $handler->display->display_options['empty']['area']['format'] = '1';
  
  ///// FIELDS /////
  
  // Field: genus
  $handler->display->display_options['fields']['genus']['id'] = 'genus';
  $handler->display->display_options['fields']['genus']['table'] = 'stock_search';
  $handler->display->display_options['fields']['genus']['field'] = 'genus';
  $handler->display->display_options['fields']['genus']['exclude'] = TRUE;

  // Field: species
  $handler->display->display_options['fields']['species']['id'] = 'species';
  $handler->display->display_options['fields']['species']['table'] = 'stock_search';
  $handler->display->display_options['fields']['species']['field'] = 'species';
  $handler->display->display_options['fields']['species']['exclude'] = TRUE;

  // Field: stock type
  $handler->display->display_options['fields']['stocktype']['id'] = 'stocktype';
  $handler->display->display_options['fields']['stocktype']['table'] = 'stock_search';
  $handler->display->display_options['fields']['stocktype']['field'] = 'stocktype';
  $handler->display->display_options['fields']['stocktype']['exclude'] = TRUE;

  // Field: stock name
  $handler->display->display_options['fields']['stockname']['id'] = 'stockname';
  $handler->display->display_options['fields']['stockname']['table'] = 'stock_search';
  $handler->display->display_options['fields']['stockname']['field'] = 'stockname';
  $handler->display->display_options['fields']['stockname']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['stockname']['alter']['path'] = 'stock/[genus]/[species]/[stocktype]/[stockname]';
  $handler->display->display_options['fields']['stockname']['alter']['replace_spaces'] = TRUE;
  
  // Field: common_name
  $handler->display->display_options['fields']['common_name']['id'] = 'common_name';
  $handler->display->display_options['fields']['common_name']['table'] = 'stock_search';
  $handler->display->display_options['fields']['common_name']['field'] = 'common_name';
  $handler->display->display_options['fields']['common_name']['label'] = 'Species';
  $handler->display->display_options['fields']['common_name']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['common_name']['alter']['path'] = 'organism/[genus]/[species]';
  
  // Field: collection
  $handler->display->display_options['fields']['collections']['id'] = 'collection';
  $handler->display->display_options['fields']['collections']['table'] = 'stock_search';
  $handler->display->display_options['fields']['collections']['field'] = 'collection';

  ///// FILTERS /////
  
  // Filter criterion: stockname
  $handler->display->display_options['filters']['stockname']['id'] = 'stockname';
  $handler->display->display_options['filters']['stockname']['table'] = 'stock_search';
  $handler->display->display_options['filters']['stockname']['field'] = 'stockname';
  $handler->display->display_options['filters']['stockname']['operator'] = 'contains';
  $handler->display->display_options['filters']['stockname']['group'] = 1;
  $handler->display->display_options['filters']['stockname']['exposed'] = TRUE;
  $handler->display->display_options['filters']['stockname']['expose']['operator_id'] = 'stockname_op';
  $handler->display->display_options['filters']['stockname']['expose']['label'] = 'Stock Name';
  $handler->display->display_options['filters']['stockname']['expose']['description'] = '(e.g. Tifrunner)';
  $handler->display->display_options['filters']['stockname']['expose']['use_operator'] = false;
  $handler->display->display_options['filters']['stockname']['expose']['operator'] = 'stockname_op';
  $handler->display->display_options['filters']['stockname']['expose']['identifier'] = 'stockname';
  $handler->display->display_options['filters']['stockname']['max_length'] = '20';
  $handler->display->display_options['filters']['stockname']['select_optional'] = TRUE;

  // Filter criterion: common_name
  $handler->display->display_options['filters']['common_name']['id'] = 'common_name';
  $handler->display->display_options['filters']['common_name']['table'] = 'stock_search';
  $handler->display->display_options['filters']['common_name']['field'] = 'common_name';
  $handler->display->display_options['filters']['common_name']['value'] = 'All';
  $handler->display->display_options['filters']['common_name']['exposed'] = TRUE;
  $handler->display->display_options['filters']['common_name']['expose']['operator_id'] = 'common_name_op';
  $handler->display->display_options['filters']['common_name']['expose']['label'] = 'Species';
  $handler->display->display_options['filters']['common_name']['expose']['operator'] = 'common_name_op';
  $handler->display->display_options['filters']['common_name']['expose']['identifier'] = 'common_name';
  $handler->display->display_options['filters']['common_name']['values_form_type'] = 'select';
  $handler->display->display_options['filters']['common_name']['select_optional'] = TRUE;
  $handler->display->display_options['filters']['common_name']['max_length'] = '40';
  $handler->display->display_options['filters']['common_name']['select_optional'] = TRUE;
  
  // Display: stock_search
  $handler = $view->new_display('page', 'search/search', 'page');
  $handler->display->display_options['path'] = 'search/stock';

  return $view;
}//legume_stock_defaultvalue_stock

