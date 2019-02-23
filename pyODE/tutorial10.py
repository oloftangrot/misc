# a ball on the ground, which can be controlled to roll
# ed paradis, jan 2007

# some code borrowed from http://pyode.sourceforge.net/tutorials/tutorial3.html

import sys, os, random, time
from math import *
from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *

import ode

############################### fucntions ##################################

# prepare_GL
def prepare_GL():
    """Prepare drawing.
    """

    # Viewport
    glViewport(0,0,640,480)

    # Initialize
    glClearColor(0.8,0.8,0.9,0)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST)
    glDisable(GL_LIGHTING)
    glEnable(GL_LIGHTING)
    glEnable(GL_NORMALIZE)
    glShadeModel(GL_FLAT)

    # Projection
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    gluPerspective (45,1.3333,0.2,20)

    # Initialize ModelView matrix
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()

    # Light source
    glLightfv(GL_LIGHT0,GL_POSITION,[0,0,1,0])
    glLightfv(GL_LIGHT0,GL_DIFFUSE,[1,1,1,1])
    glLightfv(GL_LIGHT0,GL_SPECULAR,[1,1,1,1])
    glEnable(GL_LIGHT0)

    # View transformation
    gluLookAt (2.4, 3.6, 4.8, 0.5, 0.5, 0, 0, 1, 0)

# Collision callback
def near_callback(args, geom1, geom2):
    """Callback function for the collide() method.

    This function checks if the given geoms do collide and
    creates contact joints if they do.
    """

    # Check if the objects do collide
    contacts = ode.collide(geom1, geom2)

    # Create contact joints
    world,contactgroup = args
    for c in contacts:
        c.setBounce(0.8)    # bouncyness
        c.setMu(800)       # friction for both surfaces
        j = ode.ContactJoint(world, contactgroup, c)
        j.attach(geom1.getBody(), geom2.getBody())


# create a ball
def create_ball(world, space, density, radius):
    """create a ball body and its corresponding geom."""

    #create the body  (the physical volume to simulate)
    body = ode.Body(world)
    M = ode.Mass()
    M.setSphere( density, radius)
    body.setMass(M)

    # set parameters for drawing the body
    body.shape = "ball"
    body.radius = radius

    # create a sphere geom for collision detection
    geom = ode.GeomSphere(space, radius=body.radius)
    geom.setBody(body)

    return body

# create a box
def create_box(world, space, density, lx, ly, lz):
    """Create a box body and its corresponding geom."""

    # Create body
    body = ode.Body(world)
    M = ode.Mass()
    M.setBox(density, lx, ly, lz)
    body.setMass(M)

    # Set parameters for drawing the body
    body.shape = "box"
    body.boxsize = (lx, ly, lz)

    # Create a box geom for collision detection
    geom = ode.GeomBox(space, lengths=body.boxsize)
    geom.setBody(body)

    return body


# insert objects into scene. called once at startup on the first frame
def insert_objects():
    """insert an object into the scene"""

    global bodies, j1

    # first we place the 'ball'
    body = create_ball(world, space, 1000, 0.30)
    body.setPosition( (0,1.0,0) )    # in the center, above the floor
    bodies.append(body)

    # then we place the 'stick'
    body = create_box(world, space, 1000, 0.1, 1.0, 0.1 )
    body.setPosition( (0,2,0) )
    bodies.append(body)

    j1.attach(bodies[0], bodies[1])
    j1.setAxis1( (1,0,0) )
    j1.setAxis2( (0,0,1) )
    j1.setParam( ode.ParamFMax, 400)
    j1.setParam( ode.ParamFMax2, 400)


    j1.setAnchor( (0,1,0) )

    
# draw a body. currently does balls and boxes.  I would like to also somehow
#  draw the floor
def draw_body(body):
    """draw an ODE body."""

    x,y,z = body.getPosition()
    R = body.getRotation()
    rot = [R[0], R[3], R[6], 0.,
           R[1], R[4], R[7], 0.,
           R[2], R[5], R[8], 0.,
           x, y, z, 1.0]
    glPushMatrix()
    glMultMatrixd(rot)
    if body.shape=="box":
        sx,sy,sz = body.boxsize
        glScale(sx, sy, sz)
        glutSolidCube(1)
    elif body.shape=="ball":
        rad = body.radius
        glutSolidSphere( rad, 24, 24)
    glPopMatrix()

    
############################### start ######################################

#initialize GLUT
glutInit ([])

#open a window
glutInitDisplayMode ( GLUT_RGB | GLUT_DOUBLE )

x = 0
y = 0
width = 640
height = 480
glutInitWindowPosition(x,y)
glutInitWindowSize(width, height)
glutCreateWindow ("ball simulator")

# create world object (the dynamics world)
world = ode.World()
world.setGravity( (0, -9.81, 0) )
world.setERP(0.8)   # error reduction parameter
world.setCFM(1E-5)  # constraint force mixing

# create a space object ( a collision space)
space = ode.Space()

# create a plane geom for the floor (infinite plane)
floor = ode.GeomPlane(space, (0,1,0), 0)

# a list with ODE bodies ( a body is a rigid body )
bodies = []

# a joint group for the contact joints that are generated whenever two bodies
#  collide
contactgroup = ode.JointGroup()

# crreate a joint between the ball and the stick
j1 = ode.UniversalJoint(world)

# simulation loop variables
fps = 50
dt = 1.0 / fps
running = True
lasttime = time.time()
state = 0
counter = 0


############################## glut call backs #############################

# keyboard callback for glut
def _keyfunc (c,x,y):
    sys.exit(0)

# hook the keyboard callback
glutKeyboardFunc (_keyfunc)

# draw callback for glut
def _drawfunc():
    # draw the scene
    prepare_GL()
    for b in bodies:
        draw_body(b)

    glutSwapBuffers ()

# hook the draw callback
glutDisplayFunc (_drawfunc)

# idle callback for glut
def _idlefunc():
    global counter, state, lasttime

    t = dt - (time.time() - lasttime)
    if (t>0):
        time.sleep(t)

    counter += 1

    if state==1:
        # run the ball forwards
        if counter > 200:
            state=2
            counter=0

        j1.setParam(ode.ParamVel, 3.0)
            
    elif state==2:
        # run the ball backwards
        if counter > 200:
            state=1
            counter=0

        j1.setParam(ode.ParamVel, -3.0)
    elif state==0:
        # ...and on the null state, he populated the world
        insert_objects()
        state=1
    
    glutPostRedisplay()

    # simulate n collisions per ???
    n = 2

    for i in range(n):
        # detect collisions and create contact joints
        space.collide((world,contactgroup), near_callback)

        # simulation step
        world.step(dt/n)

        # remove all contact joints 
        contactgroup.empty()

    lasttime = time.time()

# hook the glut idle callback
glutIdleFunc (_idlefunc)

#enter the glut main loop (from which we exit via the keyboard callback)
glutMainLoop()


