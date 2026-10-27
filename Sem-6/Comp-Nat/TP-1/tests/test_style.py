"""unit tests for style linter verifying r2 compliance"""

import os
import sys
import tempfile
import unittest

# add repository root to import path
repo_root = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
if repo_root not in sys.path:
    sys.path.insert(0, repo_root)

from scripts.check_style import check_file, find_python_files, main


class TestStyleLinter(unittest.TestCase):
    """test cases for verifying style linter behavior"""

    def setUp(self):
        self.tmp_dir = tempfile.mkdtemp()

    def tearDown(self):
        import shutil
        shutil.rmtree(self.tmp_dir, ignore_errors=True)

    def _write_file(self, content: str) -> str:
        # helper to write temporary python file
        fpath = os.path.join(self.tmp_dir, f"test_{len(os.listdir(self.tmp_dir))}.py")
        with open(fpath, "w", encoding="utf-8") as f:
            f.write(content)
        return fpath

    def test_compliant_code_passes(self):
        # test that casual lowercase comments and docstrings pass
        code = '''"""casual module docstring"""

def foo(x):
    """casual function docstring"""
    # compute intermediate value
    y = x + 1
    msg = "This String Has Uppercase And Period."
    return y
'''
        fpath = self._write_file(code)
        errors = check_file(fpath)
        self.assertEqual(errors, [])

    def test_uppercase_comment_fails(self):
        # test comment with uppercase characters is caught
        code = '''# This Comment Has Uppercase
x = 1
'''
        fpath = self._write_file(code)
        errors = check_file(fpath)
        self.assertTrue(any("comment has uppercase" in err for err in errors))

    def test_period_ending_comment_fails(self):
        # test comment ending with period is caught
        code = '''# casual comment ending with period.
x = 1
'''
        fpath = self._write_file(code)
        errors = check_file(fpath)
        self.assertTrue(any("comment ends with period" in err for err in errors))

    def test_uppercase_docstring_fails(self):
        # test docstring with uppercase is caught
        code = '''def foo():
    """Docstring With Uppercase"""
    return 1
'''
        fpath = self._write_file(code)
        errors = check_file(fpath)
        self.assertTrue(any("docstring has uppercase" in err for err in errors))

    def test_period_ending_docstring_fails(self):
        # test docstring ending with period is caught
        code = '''def foo():
    """casual docstring ending with period."""
    return 1
'''
        fpath = self._write_file(code)
        errors = check_file(fpath)
        self.assertTrue(any("docstring line ends with period" in err for err in errors))

    def test_multiline_docstring_period_fails(self):
        # test multiline docstring with line ending with period
        code = '''def foo():
    """first line ends with period.
    second line is okay"""
    return 1
'''
        fpath = self._write_file(code)
        errors = check_file(fpath)
        self.assertTrue(any("docstring line ends with period" in err for err in errors))

    def test_repository_codebase_passes_style(self):
        # scan actual repository codebase and ensure zero violations
        targets = [
            os.path.join(repo_root, "src"),
            os.path.join(repo_root, "scripts"),
            os.path.join(repo_root, "tests"),
        ]
        files = find_python_files(targets)
        self.assertGreater(len(files), 0)

        all_errors = []
        for fpath in files:
            errs = check_file(fpath)
            if errs:
                all_errors.extend(errs)

        self.assertEqual(all_errors, [], f"style violations found: {all_errors}")


if __name__ == "__main__":
    unittest.main()
