#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
Testing loading of various **elementary pieces** of TRUST datasets into the datamodel generated by trustpy.
"""

import os
import unittest

from trustpy.unittest_custom_methods_trust import UnittestCustomMethodsTrust
from reference_data import *
import trustpy.baseTru as BTRU
import trustpy.trust_utilities as TRUU
import xyzpy.classFactoryXyz as CLFX
import xyzpy.intFloatListXyz as IFLX
from trustpy.trust_parser import TRUSTParser, TRUSTStream, TRUSTEndOfStreamException

verbose = False

########################################################################################
class TestCase(unittest.TestCase, UnittestCustomMethodsTrust):
  """
  Testing loading of various dataset examples into the data model generated by trustpy.
  Also test that the model can reproduce the initial dataset once it has been parsed.
  """

  _test_dir = os.path.abspath(os.path.dirname(__file__))
  _models = []

  def test_000(self):
    """ Done first to set stdout verbosity tests redirection"""
    if not verbose:
      self.redirect_stdout()
    self.print_begin_test()
    CLFX.resetDefaults()
    # BTRU.initXyzClasses()

  def test_999(self):
    """ Done last to set stdout verbosity tests redirection"""
    self.print_begin_test()
    CLFX.resetDefaults()  # reset classFactoryXyz for next tests suites
    self.restore_stdout()
    # if verbose: print(self.caller_name_lineno())

  def string_test(self, cls_nam , simple_str, simplify=True):
    """ Generic test method taking the string representing the arguments of a class and parse it. """
    # Generate if needed
    self.generate_python_and_import_simple()

    # Parse the TRUST data set provided in arg
    if simplify:
      data_ex = TRUU.simplify_successive_blanks(simple_str)
    else:
      data_ex = simple_str
    tp = TRUSTParser()
    tp.tokenize(data_ex)
    stream = TRUSTStream(tp)
    TRUU.log_debug("Token list: %s" % stream.tokLow)
    TRUU.log_debug("Token list has %d items" % len(stream))
    ze_cls = CLFX.getXyzClassFromName(cls_nam)
    return stream, ze_cls.ReadFromTokens(stream)

  def generic_test_504(self, data_ex_orig, simplify=True):
    """ Generic test method taking a (piece of) dataset and testing it. 
    This is a (much) simpler version of baseTru.DataSet_Tru.ReadFromTokens()
    It returns the parser stream, and the object created.
    """
    # Generate if needed
    self.generate_python_and_import_simple()

    # Parse the TRUST data set provided in arg
    if simplify:
      data_ex = TRUU.simplify_successive_blanks(data_ex_orig)
    else:
      data_ex = data_ex_orig
    tp = TRUSTParser()
    tp.tokenize(data_ex)
    stream = TRUSTStream(tp)
    TRUU.log_debug("Token list: %s" % stream.tokLow)
    TRUU.log_debug("Token list has %d items" % len(stream))

    data_list = []  # A list representing the object found in the TRUST data file

    # Mimick (in a minimal fashion) what is done in DataSet_Tru.ReadFromTokens():
    cls_nam = stream.probeNextLow()
    ze_cls = CLFX.getXyzClassFromName(cls_nam + BTRU._TRIOU_SUFFIX)

    val = ze_cls.ReadFromTokens(stream)
    return stream, val

  #########################################################

  def xxxxtest_010(self):  # not done...
    """ trad2xyz: test TRAD_2 reading method: trad2xyz """
    self.print_begin_test()

    # Generate if needed
    self.generate_python_and_import_simple()

    # model is an instanciated Triou_All class
    import trustpy.Triou_All as TALL
    model = TALL.create_model_all(trust_packagespy_generated=self._TRUG[0])
    # model.logHelp()
    self._models.append(model)  # to use it next test

  def test_012(self):
    """ Test parsing simple string """
    data_ex = """
      # with many comments
        before
      #
      toto"""
    stream, inst = self.string_test("chaine_Tru", data_ex, simplify=False)
    BC = self._TRUG[0].chaine_Tru
    expec = BC("toto")
    self.assertEqual(expec, inst)
    self.assertTrue(stream.eof())
    # Test writing out
    res = ''.join(inst.toDatasetTokens())
    self.assertTrue(self.check_str_equality(res, data_ex).ok)
    # Test parsing quotes
    data_ex = """ system "rm -rf */toto.lml.gz" one two"""
    tp = TRUSTParser()
    tp.tokenize(data_ex)
    stream = TRUSTStream(tp)
    self.assertEqual(stream.tokLow, ['', 'system', '"rm-rf*/toto.lml.gz"', 'one', 'two'])
    # Test inner braces in 'chaine' (what makes 'bloc_lecture' work!)
    data_ex = """
      # with many comments
        before
      #
      { ta tu { toto } bouh }"""
    stream, inst = self.string_test("chaine_Tru", data_ex, simplify=False)
    expec = BC(data_ex)
    self.assertEqual(expec, inst)
    self.assertTrue(stream.eof())
    # Test writing out
    res = ''.join(inst.toDatasetTokens())
    self.assertTrue(self.check_str_equality(res, data_ex).ok)
    # Misformatted '{' should fail:
    data_ex = "} toto }"
    # stream, inst = self.string_test("chaine_Tru", data_ex, simplify=False)
    self.assertRaises(ValueError, self.string_test, "chaine_Tru", data_ex)
    data_ex = "   {toto }"
    # stream, inst = self.string_test("chaine_Tru", data_ex, simplify=False)
    self.assertRaises(ValueError, self.string_test, "chaine_Tru", data_ex)

  def test_014(self):
    """ Test EOF functionnality """
    data_ex = """ # with comment #   toto      """
    stream, _ = self.string_test("BaseChaine_Tru", data_ex, simplify=False)
    self.assertTrue(stream.eof())

  def test_015(self):
    """ Test parsing simple float list """
    self.print_begin_test()

    # Correct list
    data_ex = "3 48.5 89.2 18"
    stream, inst = self.string_test("list_Tru", data_ex)
    LT, BF = self._TRUG[0].list_Tru, self._TRUG[0].BaseFloattant_Tru
    expec = LT()
    expec.extend([BF(48.5), BF(89.2), BF(18.0)])
    self.assertTrue(expec.equal(inst))
    self.assertTrue(stream.eof())

    # Test writing out:
    res = ''.join(inst.toDatasetTokens())
    self.assertTrue(self.check_str_equality(res, data_ex).ok)

    # Changing value on the model side should change output, and not reproduce initial tokens!!
    inst[1] = BF(39.3)
    new_s = "3 48.5 39.3 18"
    res = ''.join(inst.toDatasetTokens())
    self.assertEqual(res, new_s)
    inst.pop()
    new_s = " 2 48.5 39.3"  # yes a space to start with ...
    res = ''.join(inst.toDatasetTokens())
    self.assertEqual(res, new_s)

    # Same thing stressing parser:
    data_ex = """3   48.5
     89.2 # comment inside #  18"""
    stream, inst = self.string_test("list_Tru", data_ex, simplify=False)  # warning: no simplification here
    self.assertTrue(expec.equal(inst))
    self.assertTrue(stream.eof())
    s = ''.join(inst.toDatasetTokens())
    self.assertEqual(s, data_ex)

    # Changing value on the model side should change output, and not reproduce initial tokens!!
    inst[0] = BF(39.3)
    new_s = """3 39.3
     89.2 # comment inside #  18"""
    res = ''.join(inst.toDatasetTokens())
    self.assertEqual(res, new_s)
    inst.pop()
    new_s = """ 2 39.3
     89.2""" # yes a space to start with ...
    res = ''.join(inst.toDatasetTokens())
    self.assertEqual(res, new_s)

    # Ill formed lists
    data_ex = "3 48.5 89.2"
    #offset, inst = self.string_test("list_Tru", data_ex)
    self.assertRaises(TRUSTEndOfStreamException, self.string_test, "list_Tru", data_ex)
    data_ex = "3 48.5 89.2 sfsf"
    #offset, inst = self.string_test("list_Tru", data_ex)
    self.assertRaises(ValueError, self.string_test, "list_Tru", data_ex)

  def test_020(self):
    """ Test parsing and loading of a minimal TRUST dataset using curly braces """
    self.print_begin_test()
    data_ex = """
    # Some stupid test #
    read_MED_bidon 
      {
         mesh ze_mesh_name
         file a/complicated/path/to.med
         exclude_groups 2 toto titi
         convertAllToPoly
      }"""
    for simplify in [True, False]:
      stream, res = self.generic_test_504(data_ex, simplify=simplify)
      exp = buildCurlyExpec(self._TRUG[0])
      self.assertTrue(exp.equal(res))
      self.assertTrue(stream.eof())
      if not simplify:
        # Testing writing out
        s = ''.join(res.toDatasetTokens())
        self.assertTrue(self.check_str_equality(s, data_ex).ok)

    # Modifying data should change output!!
    res.convertalltopoly = False
    res.file = "new/file.med"
    s = ''.join(res.toDatasetTokens())
    new_s = """
    # Some stupid test #
    read_MED_bidon 
      {
         mesh ze_mesh_name
         file new/file.med
         exclude_groups 2 toto titi
      }"""
    self.assertTrue(self.check_str_equality(s, new_s).ok)

    # Ill-formed dataset - missing brace
    data_ex = """
    # Some stupid test #
    read_MED_bidon 
      
         mesh ze_mesh_name
         file a/complicated/path/to.med
         exclude_groups 2 toto titi
         convertAllToPoly
      }"""
    # self.generic_test_504(data_ex)
    self.assertRaises(ValueError, self.generic_test_504, data_ex)
    # Ill-formed dataset - missing brace
    data_ex = """
    # Some stupid test #
    read_MED_bidon 
      {
         mesh ze_mesh_name
         file a/complicated/path/to.med
         exclude_groups 2 toto titi
         convertAllToPoly"""
    self.assertRaises(TRUSTEndOfStreamException, self.generic_test_504, data_ex)

  def test_030(self):
    """ Basic test - stressing parser """
    self.print_begin_test()
    data_ex = """read_MED_bidon { mesh ze_mesh_name file a/complicated/path/to.med exclude_groups 2 toto titi convertAllToPoly }"""
    stream, res = self.generic_test_504(data_ex, simplify=False)
    exp = buildCurlyExpec(self._TRUG[0])
    self.assertTrue(exp.equal(res))
    self.assertTrue(stream.eof())
    # Test writing out:
    s = ''.join(res.toDatasetTokens())
    self.assertTrue(self.check_str_equality(s, data_ex).ok)

    data_ex += " toto"  # should still parse, 'toto' being the next keyword in the stream
    stream, res = self.generic_test_504(data_ex, simplify=False)
    self.assertTrue(exp.equal(res))
    self.assertFalse(stream.eof())  # but not end of stream

  def test_035(self):
    """ Test attribute optionality. Missing required attribute should fail the parse. """
    self.print_begin_test()
    data_ex = """
    # Invalid data with missing required attribute (file) #
    read_MED_bidon 
      {
         mesh ze_mesh_name
         # file a/complicated/path/to.med #
         exclude_groups 2 toto titi
         convertAllToPoly
      }
    """
    # Should fail since mandatory attribue 'file' is missing:
    self.assertRaises(ValueError, self.generic_test_504, data_ex)

    # Keyword with all optionals should accept no attributes at all:
    data_ex = """coucou { }"""
    stream, res = self.generic_test_504(data_ex)
    MB = self._TRUG[0].coucou_Tru
    expec = MB()
    self.assertTrue(expec.equal(res))
    self.assertTrue(stream.eof())
    # Test writing out:
    s = ''.join(res.toDatasetTokens())
    self.assertTrue(self.check_str_equality(s, data_ex).ok)

  def test_040(self):
    """ Test synonyms """
    self.print_begin_test()
    data_ex = """
    # Some stupid test with syno #
    read_MED_bidon 
      {
         maillage ze_mesh_name
         fichier a/complicated/path/to.med
         exclude_groups 2 toto titi
         convertAllToPoly
      }"""
    for simplify in [True, False]:
      stream, res = self.generic_test_504(data_ex, simplify=simplify)
      exp = buildCurlyExpec(self._TRUG[0])
      self.assertTrue(exp.equal(res))
      self.assertTrue(stream.eof())
      if not simplify:
        # Test writing out:
        s = ''.join(res.toDatasetTokens())
        self.assertTrue(self.check_str_equality(s, data_ex).ok)

  def test_050(self):
    """ Testing keywords with no curly braces """
    self.print_begin_test()
    # Standard case
    data_ex = """uniform_field 3 34.6 12.8 90.9"""
    stream, res = self.generic_test_504(data_ex)
    UF, LT, BF = self._TRUG[0].uniform_field_Tru, self._TRUG[0].list_Tru, self._TRUG[0].BaseFloattant_Tru
    # Build expected value
    exp, l = UF(), LT()
    l.extend([BF(34.6), BF(12.8), BF(90.9)])
    exp.val = l
    self.assertTrue(res.equal(exp))
    self.assertTrue(stream.eof())
    # Test writing out:
    s = ''.join(res.toDatasetTokens())
    self.assertTrue(self.check_str_equality(s, data_ex).ok)

    # The below should parse - 'toto' can be considered the next keyword in the stream
    data_ex2 = data_ex + " toto "
    stream, res = self.generic_test_504(data_ex2)
    self.assertTrue(res.equal(exp))
    #   here we re not at EOF, since 'toto' is there
    self.assertFalse(stream.eof())
    # With an optional flag and more spaces in between!
    data_ex = """
    # Example no curly brace with optional #
    uniform_field   flag_bidon  3   34.6  12.8 90.9 
       flag_bidon2  2 15   29"""
    for simplify in [True, False]:
      stream, res = self.generic_test_504(data_ex, simplify=simplify)
      exp, l, l2 = UF(), LT(), LT()
      l.extend([BF(34.6), BF(12.8), BF(90.9)])
      l2.extend([BF(15), BF(29)])
      exp.val, exp.val2 = l, l2
      exp.flag_bidon, exp.flag_bidon2 = "flag_bidon", "flag_bidon2"
      self.assertTrue(res.equal(exp))
      self.assertTrue(stream.eof())
      if not simplify:
        # Test writing out:
        s = ''.join(res.toDatasetTokens())
        self.assertTrue(self.check_str_equality(s, data_ex).ok)

    # Same story with next keyword:
    data_ex2 = data_ex + " toto "
    stream, res = self.generic_test_504(data_ex2)
    self.assertTrue(res.equal(exp))
    self.assertFalse(stream.eof())

    # Ill-formed dataset (missing attribute):
    data_ex = """
    # Ill-formed #
    uniform_field flag_bidon
    """
    # self.generic_test_504(data_ex)
    self.assertRaises(TRUSTEndOfStreamException, self.generic_test_504, data_ex)
    # Ill-formed dataset (missing values)
    data_ex = """
    # Ill-formed #
    uniform_field 3 34.6 12.8
    """
    # self.generic_test_504(data_ex)
    self.assertRaises(TRUSTEndOfStreamException, self.generic_test_504, data_ex)

  def test_060(self):
    """ Testing complex attributes (with a prescribed type) """
    self.print_begin_test()
    data_ex = """
      coucou {
        attr_bidon champ_uniforme 2 0 9.8
      }"""
    for simplify in [True, False]:
      stream, res = self.generic_test_504(data_ex, simplify=simplify)
      MB = self._TRUG[0].coucou_Tru
      UF, LT, BF = self._TRUG[0].uniform_field_Tru, self._TRUG[0].list_Tru, self._TRUG[0].BaseFloattant_Tru
      # Build expected value
      exp, uf, l = MB(), UF(), LT()
      l.extend([BF(0), BF(9.8)])
      uf.val = l
      exp.attr_bidon = uf

      self.assertTrue(res.equal(exp))
      self.assertTrue(stream.eof())
      if not simplify:
        # Test writing out:
        s = ''.join(res.toDatasetTokens())
        self.assertTrue(self.check_str_equality(s, data_ex).ok)

  def test_070(self):
    """ Testing adding an attribute to a keyword and checking output ok """
    self.print_begin_test()
    data_ex = """
    # Some stupid test with syno #
    read_MED_bidon 
      {
         maillage ze_mesh_name
         fichier a/complicated/path/to.med
         exclude_groups 2 toto titi
         convertAllToPoly
      }"""

    stream, res = self.generic_test_504(data_ex, simplify=False)
    exp = buildCurlyExpec(self._TRUG[0])
    self.assertTrue(exp.equal(res))
    self.assertTrue(stream.eof())
    # Adding a (valid) attribute
    res.no_family_names_from_group_names = True
    data_expected = """
    # Some stupid test with syno #
    read_MED_bidon 
      {
         maillage ze_mesh_name
         fichier a/complicated/path/to.med
         exclude_groups 2 toto titi
         convertAllToPoly
no_family_names_from_group_names 
      }"""  # Yes I know this is poorly indented ...
    # Test writing out:
    s = ''.join(res.toDatasetTokens())
    self.assertTrue(self.check_str_equality(s, data_expected).ok)

  def test_200(self):
    """ Testing inheritance - gravite expects a 'field_base' of which 'champ_uniform' is a child 
    """
    self.print_begin_test()
    data_ex = """
      coucou {
        gravite champ_uniforme 2 0 9.8
      }"""
    for simplify in [True, False]:
      stream, res = self.generic_test_504(data_ex, simplify=simplify)
      MB = self._TRUG[0].coucou_Tru
      UF, LT, BF = self._TRUG[0].uniform_field_Tru, self._TRUG[0].list_Tru, self._TRUG[0].BaseFloattant_Tru
      # Build expected value
      exp, uf, l = MB(), UF(), LT()
      l.extend([BF(0), BF(9.8)])
      uf.val = l
      exp.gravite = uf
      # Test
      self.assertTrue(res.equal(exp))
      self.assertTrue(stream.eof())
      if not simplify:
        # Test writing out:
        s = ''.join(res.toDatasetTokens())
        self.assertTrue(self.check_str_equality(s, data_ex).ok)

    # Changing inherited attribute should change output:
    FB = self._TRUG[0].field_base_Tru
    res.gravite = FB()
    s = ''.join(res.toDatasetTokens())
    new_s = """
      coucou {
        gravite field_base
      }"""
    self.assertTrue(self.check_str_equality(s, new_s).ok)

    # Wrong inheritance should raise:
    data_ex = """
      coucou {
        gravite coucou { }
      }
    """
    # offset, res = self.generic_test_504(data_ex)
    self.assertRaises(Exception, self.generic_test_504, data_ex)

  def test_400(self):
    """ Testing forward declaration and 'read' keyword.
    This uses the main loop from DataSet_Tru. """
    self.print_begin_test()
    data_ex = """
    lire_med_bidon   rmed    # bidon because not an interpret #
    coucou cb
    
    Read  rmed 
      {
         mesh ze_mesh_name
         file a/complicated/path/to.med
      }
    
    read    cb {   }"""
    # Generate if needed
    self.generate_python_and_import_simple()
    tds_cls = CLFX.getXyzClassFromName("DataSet" + BTRU._TRIOU_SUFFIX)
    # Parse the TRUST data set provided in arg
    tp = TRUSTParser()

    for simplify in [True, False]:
      if simplify:
        data_ex_mod = TRUU.simplify_successive_blanks(data_ex)
      else:
        data_ex_mod = data_ex
      tp.tokenize(data_ex_mod)
      stream = TRUSTStream(tp)
      TRUU.log_debug("Token list: %s" % stream.tokLow)
      TRUU.log_debug("Token list has %d items" % len(stream))
      res = tds_cls.ReadFromTokens(stream)
      self.assertEqual(len(res), 4)
      self.assertTrue(stream.eof())
      exp0, exp1, exp2, exp3 = buildForwardExpec(self._TRUG[0])
      self.assertTrue(exp0.equal(res[0])) # Forward decl
      self.assertTrue(exp1.equal(res[1])) # Coucou cb
      self.assertTrue(exp2.equal(res[2])) # read rmed
      self.assertTrue(exp3.equal(res[3])) # read cb
      if not simplify:
        # Test writing out:
        s = ''.join(res.toDatasetTokens())
        self.assertTrue(self.check_str_equality(s, data_ex).ok)

  def test_410(self):
    """ Forward declaration test. 
    This uses the main loop from DataSet_Tru. """
    self.print_begin_test()
    # Valid dataset
    data_ex = """champ_uniforme gravite
                 read gravite 2 0.0 9.8"""
    self.generate_python_and_import_simple()
    tds_cls = CLFX.getXyzClassFromName("DataSet" + BTRU._TRIOU_SUFFIX)
    tp = TRUSTParser()
    tp.tokenize(data_ex)
    stream = TRUSTStream(tp)
    res = tds_cls.ReadFromTokens(stream)
    self.assertEqual(len(res), 2)
    self.assertTrue(stream.eof())
    UF, D, LF = self._TRUG[0].uniform_field_Tru, self._TRUG[0].Declaration_Tru, self._TRUG[0].ListOfFloat_Tru
    decl = D()
    decl.identifier, decl.cls_nam = "gravite", "champ_uniforme_Tru"
    f, l = UF(), LF([0,9.8])
    f.val = l
    self.assertTrue(res[0].equal(decl))
    self.assertTrue(res[1].equal(f))
    # Test writing out:
    s = ''.join(res.toDatasetTokens())
    self.assertTrue(self.check_str_equality(s, data_ex).ok)

    # Invalid datasets
    #    Misformatted fwd decl:
    data_ex = """champ_uniforme 35
                 coucou { }"""
    tp.tokenize(data_ex)
    stream = TRUSTStream(tp)
    # res = tds_cls.ReadFromTokens(stream)
    self.assertRaises(ValueError, tds_cls.ReadFromTokens, stream)
    #    Misformatted - trying to make a forward declaration with an interprete:
    data_ex = """champ_uniforme gravite
                 lire_med forward"""
    tp.tokenize(data_ex)
    stream = TRUSTStream(tp)
    # res = tds_cls.ReadFromTokens(stream)
    self.assertRaises(ValueError, tds_cls.ReadFromTokens, stream)

  def test_500(self):
    """ Complete dataset with foward declarations etc... """
    self.print_begin_test()
    data_ex = """
    # Some stupid test #
    champ_uniforme gravite 
    nom coucou
    read gravite 2 28 32   # Keyword with no brace #
    read coucou toto
    lire_MED
      {
         mesh ze_mesh_name
         file a/complicated/path/to.med
         exclude_groups 2 toto titi
         convertAllToPoly
      }"""

    # Generate if needed
    self.generate_python_and_import_simple()
    tds_cls = CLFX.getXyzClassFromName("DataSet" + BTRU._TRIOU_SUFFIX)
    # Parse the TRUST data set provided in arg
    tp = TRUSTParser()

    for simplify in [True, False]:
      if simplify:
        data_ex_mod = TRUU.simplify_successive_blanks(data_ex)
      else:
        data_ex_mod = data_ex
      tp.tokenize(data_ex_mod)
      stream = TRUSTStream(tp)
      TRUU.log_debug("Token list: %s" % stream.tokLow)
      TRUU.log_debug("Token list has %d items" % len(stream))
      res = tds_cls.ReadFromTokens(stream)
      self.assertEqual(len(res), 5)
      self.assertTrue(stream.eof())

      exp = buildMinimalExpec(self._TRUG[0])
      for i in range(5):
        self.assertTrue(exp[i].equal(res[i]))
      if not simplify:
        # Test writing out:
        s = ''.join(res.toDatasetTokens())
        self.assertTrue(self.check_str_equality(s, data_ex).ok)

if __name__ == '__main__':
  verbose = True  # verbose if in main
  TRUU._log_debug = False
  timeout = None
  unittest.main()