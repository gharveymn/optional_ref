import gdb

def register_printers(obj):
  from .prettyprinter import gch_optional_ref_printer
  gdb.printing.register_pretty_printer(obj, gch_optional_ref_printer)

register_printers(None)
