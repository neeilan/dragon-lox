import os
import subprocess
import sys
import unittest

def absolute_path(relative_path):
  return os.path.join(os.path.abspath(sys.path[0]), relative_path) 

LOXPP_PATH = absolute_path('../bin/loxpp') 

def run_file(path):
  result = subprocess.run([LOXPP_PATH, path], stdout=subprocess.PIPE)
  return result.stdout.decode('utf-8').strip()

 
class LoxppOutputTest(unittest.TestCase):
 
    def setUp(self):
        pass
 
    def test_fibonacci(self):
      expected = 'Interpreter output:\n8'
      output = run_file(absolute_path('fibonacci.lox'))
      self.assertEqual(expected, output)

    def test_function_returns(self):
      expected = ("Interpreter output:\n"
        "20\n15\nadding strings!\n"
        "Returning nil explicitly\nnil\n"
        "Returning nil implicitly\nnil")
        
      output = run_file(absolute_path('fn_returns.lox'))
      self.assertEqual(expected, output)
        
    def test_while_loop(self):
      expected = 'Interpreter output:\n0\n1\n2\n3\n4'
      output = run_file(absolute_path('simple_while_loop.lox'))
      self.assertEqual(expected, output)
    
    def test_for_loop(self):
      expected = 'Interpreter output:\n0\n2\n4\n6\n8'
      output = run_file(absolute_path('for_loop.lox'))
      self.assertEqual(expected, output)
    
    def test_closure(self):
      expected = 'Interpreter output:\n1\n2'
      output = run_file(absolute_path('closure.lox'))
      self.assertEqual(expected, output)
    
    def test_this_bound_closure(self):
      expected = 'Interpreter output:\n<fn localFunction>\n<fn localFunction>'
      output = run_file(absolute_path('this_closure.lox'))
      self.assertEqual(expected, output)
 
    def test_recursive_countdown(self):
      expected = 'Interpreter output:\n3\n2\n1\n0'
      output = run_file(absolute_path('recursive_countdown.lox'))
      self.assertEqual(expected, output)

    def test_call_methods_during_init(self):
      expected = 'Interpreter output:\n3\n-2'
      output = run_file(absolute_path('call_methods_during_init.lox'))
      self.assertEqual(expected, output)

    def test_init_returns_instance(self):
      expected = 'Interpreter output:\n<SomeClass instance>\n<SomeClass instance>'
      output = run_file(absolute_path('init_returns_this.lox'))
      self.assertEqual(expected, output)
 
    def test_init_allows_early_return(self):
      expected = 'Interpreter output:\n10\n5'
      output = run_file(absolute_path('init_allows_early_return.lox'))
      self.assertEqual(expected, output)

    def test_superclass_method_call_on_instance(self):
      expected = 'Interpreter output:\nFry until golden brown.'
      output = run_file(absolute_path('superclass_method.lox'))
      self.assertEqual(expected, output)

    def test_multiline_comments(self):
      expected = 'Interpreter output:\nMultiline comments!'
      output = run_file(absolute_path('multiline_comments.lox'))
      self.assertEqual(expected, output)

    def test_simple_lambdas(self):
        expected = 'Interpreter output:\n2\n4\n6\n6\n-1\n-2\n-3\n2\n4\n6\n-3\n-6\n-9\n1\n2\n3'
        output = run_file(absolute_path('simple_lambdas.lox'))
        self.assertEqual(expected, output)


class LoxppOutputErrorsTest(unittest.TestCase):
 
    def setUp(self):
        pass
 
    def test_top_level_return(self):
      error_snippets = ['line 5', 'Cannot return from top-level code']
      output = run_file(absolute_path('err_top_level_return.lox'))
      
      for snippet in error_snippets:
        self.assertTrue(snippet in output)

    def test_return_from_init(self):
      error_snippets = ['line 4', 'Cannot return a value from initializer.']
      output = run_file(absolute_path('err_return_from_init.lox'))
      
      for snippet in error_snippets:
        self.assertTrue(snippet in output)


    def test_redeclared_vars(self):
      error_snippets = ['line 6', 'line 13', "'a'", "'c'", 'already declared in this scope']
      output = run_file(absolute_path('err_scope_redeclarations.lox'))
      
      for snippet in error_snippets:
        self.assertTrue(snippet in output)
 

if __name__ == '__main__':
  unittest.main()
