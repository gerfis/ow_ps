<?php
/**
 * function to get all term ids inside a vocab in a flat array
 * No matter if vocab consists of hierarichal order of terms,you will get all term ids in a flat array.
 * @param
 *   $vocab_machine_name takes vocabulary machine name as parameter.
 * @return
 *   $tids return array of tids in a flat array format
 */
function get_taxonomy_term_ids_inside_vocab($vocab_machine_name) {
  $vocab = taxonomy_vocabulary_machine_name_load($vocab_machine_name);
  if (is_object($vocab)) {
    $term_tree = taxonomy_get_tree($vocab->vid);
    if (is_array($term_tree) && !empty($term_tree)) {
      foreach ($term_tree as $key => $value) {
        $tids[] = $value->tid;
      }
    } if (empty($term_tree)) {
      $tids = array();
    }
  } else
    return "No Vocab Found with the given name";

  return ($tids);
}

/**
 * function to get all the node ids directly mapped to the given taxonomy terms.
 * Returns Flat array of node ids belong to the given term ids.
 * @param
 *   $tid takes term id can be a single term or array of terms.
 * @return
 *   $nid/$nids returns node ids array or sinly node id directly mapped to the given taxonomy term.
 */
function get_nodes_directly_mapped_to_term($tid) {
  if (is_array($tid) && !empty($tid)) {
    foreach ($tid as $key => $value) {
      $nids[] = taxonomy_select_nodes($value);
      $flat_nids = multitosingle($nids);
    }
    return $flat_nids;
  }
  else {
    $nid = taxonomy_select_nodes($tid);
    return $nid;
  }
}

/**
 * function to convert mutlidimension arrays to flat arrays.
 * Returns Flat arrays.
 * @param
 *   $input takes input array as multudimension array.
 *   $output takes output array as paramter in the recursive call.
 * @return
 *   $output return flat array format for the input multidimenional array
 */
function multitosingle($input, $output = array()) {
  ksort($input);
  foreach ($input as $value) {
    $key = count($output);
    if (is_array($value)) {
      $output = multitosingle($value, $output);
    }
    else {
      $output[$key] = $value;
    }
  }
  return $output;
}

/**
 * function to get array/single nids which are attached with any of the taxonomy term of the given vocab.
 * Returns Flat array of nids.
 * @param
 *   $vocab_machine_name takes the machine name for the vocabulary.
 * @return
 *   $nids_inside_vocab returns array of nids/single nid attached to any taxonomy term under the given vocab.
 */
function get_nodes_belong_to_vocab($vocab_machine_name) {
  $vocab_tids = get_taxonomy_term_ids_inside_vocab($vocab_machine_name);
  if (is_array($vocab_tids) && count($vocab_tids) > 0) {
    $nids_inside_vocab = get_nodes_directly_mapped_to_term($vocab_tids);
    return $nids_inside_vocab;
  }
  elseif (is_array($vocab_tids) && count($vocab_tids) == 0) {
    return array();
  }
  else {
    return "There is no vocab found with the given machine name";
  }
}

/** 
 * function to get array/single nids inside a single taxonomy term/array of taxonomy terms
 * function to get all node ids recursively inside a taxonomy term.
 * if parent term is passed as parameter then the function returns nid belongs to the parent term and nids related to its child terms.
 * Returns Flat array of nids.
 * @param
 *   $tids takes array/single tids for which you need nids recursively.
 * @return
 *   $nids flat array of nids
 */
function get_all_nodes_belong_to_taxonomy_hierarchy($tids) {
  if (is_array($tids)) {
    foreach ($tids as $key => $value) {
      if ($value <= 0) {
        return "Please Enter a valid taxonomy term id set";
      }
    }
  }
  if (is_array($tids) && !empty($tids)) {
    foreach ($tids as $key => $value) {
      $term = taxonomy_term_load($value);
      $hierarchical_terms_object[] = taxonomy_get_tree($term->vid, $term->tid);
    }
    $flat_terms_object = multitosingle($hierarchical_terms_object);
    if (is_array($flat_terms_object) && !empty($flat_terms_object)) {
      foreach ($flat_terms_object as $key => $value) {
        $flat_tids[] = $value->tid;
      }
      $flat_tids[] = $tids;
      return array_unique(get_nodes_directly_mapped_to_term($flat_tids));
    }
    else {
      $flat_tids = $tids;
      return array_unique(get_nodes_directly_mapped_to_term($flat_tids));
    }
  }
  elseif (!is_array($tids) && $tids > 0) {
    $flat_tids = $tids;
    return get_all_nodes_belong_to_taxonomy_hierarchy(array($flat_tids));
  }
  else {
    return "Please Enter a valid term id";
  }
}

?>