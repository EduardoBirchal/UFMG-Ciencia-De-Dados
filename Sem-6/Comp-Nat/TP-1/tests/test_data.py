"""unit tests for dataset ingestion module"""

import os
import shutil
import sys
import tempfile
import unittest
import numpy as np

# add repository root to import path
repo_root = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
if repo_root not in sys.path:
    sys.path.insert(0, repo_root)

from src.data import load_dataset


class TestDataIngestion(unittest.TestCase):
    """test cases for verifying data loading and validation"""

    def setUp(self):
        # determine dataset root relative to test file location
        self.base_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))

    def test_load_synth1_shapes_and_ground_truth(self):
        # test loading synth1 and verify exact polynomial target
        x_tr, y_tr, x_te, y_te = load_dataset("synth1", base_dir=self.base_dir)

        self.assertEqual(x_tr.shape, (60, 2))
        self.assertEqual(y_tr.shape, (60,))
        self.assertEqual(x_te.shape, (600, 2))
        self.assertEqual(y_te.shape, (600,))

        self.assertEqual(x_tr.dtype, np.float64)
        self.assertEqual(y_tr.dtype, np.float64)
        self.assertEqual(x_te.dtype, np.float64)
        self.assertEqual(y_te.dtype, np.float64)

        self.assertTrue(x_tr.flags.c_contiguous)
        self.assertTrue(y_tr.flags.c_contiguous)
        self.assertTrue(x_te.flags.c_contiguous)
        self.assertTrue(y_te.flags.c_contiguous)

        self.assertFalse(np.isnan(x_tr).any())
        self.assertFalse(np.isnan(y_tr).any())
        self.assertFalse(np.isnan(x_te).any())
        self.assertFalse(np.isnan(y_te).any())

        # synth1 ground truth is y = x0^4 - x0^3 + 0.5 * x1^2 - x1
        y_tr_expected = x_tr[:, 0] ** 4 - x_tr[:, 0] ** 3 + 0.5 * (x_tr[:, 1] ** 2) - x_tr[:, 1]
        tr_rmse = np.sqrt(np.mean((y_tr - y_tr_expected) ** 2))
        self.assertLess(tr_rmse, 1e-12)

        y_te_expected = x_te[:, 0] ** 4 - x_te[:, 0] ** 3 + 0.5 * (x_te[:, 1] ** 2) - x_te[:, 1]
        te_rmse = np.sqrt(np.mean((y_te - y_te_expected) ** 2))
        self.assertLess(te_rmse, 1e-12)

    def test_load_synth2_shapes_and_properties(self):
        # test loading synth2 dataset
        x_tr, y_tr, x_te, y_te = load_dataset("synth2", base_dir=self.base_dir)

        self.assertEqual(x_tr.shape, (300, 2))
        self.assertEqual(y_tr.shape, (300,))
        self.assertEqual(x_te.shape, (1000, 2))
        self.assertEqual(y_te.shape, (1000,))

        self.assertTrue(x_tr.flags.c_contiguous)
        self.assertTrue(y_tr.flags.c_contiguous)
        self.assertFalse(np.isnan(x_tr).any())
        self.assertFalse(np.isnan(y_tr).any())

        # check target bounds are within expected range
        self.assertGreater(float(np.min(y_tr)), -15.0)
        self.assertLess(float(np.max(y_tr)), 15.0)

    def test_load_concrete_shapes_and_properties(self):
        # test loading concrete dataset
        x_tr, y_tr, x_te, y_te = load_dataset("concrete", base_dir=self.base_dir)

        self.assertEqual(x_tr.shape, (824, 8))
        self.assertEqual(y_tr.shape, (824,))
        self.assertEqual(x_te.shape, (206, 8))
        self.assertEqual(y_te.shape, (206,))

        self.assertTrue(x_tr.flags.c_contiguous)
        self.assertTrue(y_tr.flags.c_contiguous)
        self.assertFalse(np.isnan(x_tr).any())
        self.assertFalse(np.isnan(y_tr).any())

        # compressive strength is positive and bounded
        self.assertGreater(float(np.min(y_tr)), 0.0)
        self.assertLess(float(np.max(y_tr)), 100.0)

    def test_missing_file_raises(self):
        # non existent base directory must raise file not found error
        with self.assertRaises(FileNotFoundError):
            load_dataset("synth1", base_dir="non_existent_directory_98765")


if __name__ == "__main__":
    unittest.main()

