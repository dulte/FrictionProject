import ovito
from ovito.io import import_file
from ovito.vis import Viewport, RenderSettings
import sys

name = sys.argv[1]
cols = ['Particle Type', 'Position.X', 'Position.Y']
node = import_file(name, columns=cols)
node.add_to_scene()

rs = RenderSettings(
    filename=name[:name.find('.')] + '.png',
    size=(1024, 768),
    # background_color=(0.4, 0.4, 0.4)
    generate_alpha=True
)
vp = ovito.dataset.viewports.active_vp
vp.type = Viewport.Type.TOP
vp.zoom_all()
vp.render(rs)
