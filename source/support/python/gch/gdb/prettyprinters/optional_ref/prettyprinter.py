import gdb

class GCHOptionalRefPrinter(object):
  'Print a gch::optional_ref'

  def __init__(self, val):
    self.val = val
    self.ptr = val['m_ptr']
    if self.ptr:
      self.maybe_ref = self.ptr.dereference().reference_value()
      self.maybe_ref_pp = gdb.default_visualizer(self.maybe_ref)
    else:
      self.maybe_ref = None
      self.maybe_ref_pp = None

  class _iterator(object):
    def __init__(self, ref):
      self.ref = ref

    def __iter__(self):
      return self

    def __next__(self):
      if self.ref is None:
        raise StopIteration
      curr = self.ref
      self.ref = None
      return ('[contained reference]', curr)

  def children(self):
    if self.maybe_ref is None:
      return self._iterator(None)
    if hasattr(self.maybe_ref_pp, 'children'):
      return self.maybe_ref_pp.children()
    return self._iterator(self.maybe_ref)

  def to_string(self):
    if not self.ptr:
      return f'{self.val.type} [no contained reference]'
    if hasattr(self.maybe_ref_pp, 'children'):
      return f'{self.val.type} containing {self.maybe_ref_pp.to_string()}'
    return self.val.type

  def display_hint(self):
    if hasattr(self.maybe_ref_pp, 'children') and hasattr(self.maybe_ref_pp, 'display_hint'):
      return self.maybe_ref_pp.display_hint()
    return None

def build_printer():
  pp = gdb.printing.RegexpCollectionPrettyPrinter('gch::optional_ref')
  pp.add_printer('gch::optional_ref', '^gch::optional_ref<.*>$', GCHOptionalRefPrinter)
  return pp

gch_optional_ref_printer = build_printer()
