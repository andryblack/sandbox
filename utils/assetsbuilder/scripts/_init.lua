require '_base'

register_rule(require '_rules')
register_rule(require '_convert_spine')
register_rule(require '_images')

dofile('_extensions.lua')

dofile('_run.lua')