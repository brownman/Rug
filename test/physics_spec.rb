require File.dirname(__FILE__) + '/../lib/Physics'

include Rug
include Rug::Physics

describe "Collision detection" do
  before :all do
    @body1 = Body.new 0.0, 20, 20
    @body2 = Body.new 0.0, 15, 15
    @body3 = Body.new 0.0, 100, 100

    @rect = Rectangle.new 10, 10
    @body1.shape = @rect
    @circ = Circle.new 10
    @body2.shape = @circ
    @circ2 = Circle.new 10
    @body3.shape = @circ2
  end

  it "tests line circle intersection" do
    Physics.line_circle_intersect(@circ, 15, 0, 15, 30).should == true
    Physics.line_circle_intersect(@circ, 0, 20, 30, 20).should == true

    Physics.line_circle_intersect(@circ, 30, 0, 30, 30).should_not == true
    Physics.line_circle_intersect(@circ, 0, 30, 30, 30).should_not == true

    # test a line that ends within the circle
    Physics.line_circle_intersect(@circ, 0, 0, 10, 10).should == true

    # test a line that is going towards the circle, but doesn't cross it
    Physics.line_circle_intersect(@circ, 30, 30, 40, 40).should_not == true
  end

  it "should collide with rect" do
    Physics.point_in_rect(@rect, 25, 25).should == true
  end

  it "should not collide with rect" do
    Physics.point_in_rect(@rect, 15, 25).should_not == true
    Physics.point_in_rect(@rect, 25, 15).should_not == true
    Physics.point_in_rect(@rect, 35, 25).should_not == true
    Physics.point_in_rect(@rect, 25, 35).should_not == true
  end

  it "should collide with circle" do
    Physics.point_in_circle(@circ, 20, 20).should == true
  end

  it "should not collide with circle" do
    Physics.point_in_circle(@circ, 25.1, 15).should_not == true
  end

  it "should overlap" do
    @body1.overlap(@body2).should == true

    # test intersection with bottom left
    @body2.y = 35

    @body1.overlap(@body2).should == true
  end

  it "should overlap - small circle" do
    # create a small circle near the rectangle, it should overlap
    body = Body.new 0.0, 18, 25
    body.shape = Circle.new 4

    @body1.overlap(body).should == true
  end

  it "should not overlap" do
    @body1.overlap(@body3).should_not == true
    @body2.overlap(@body3).should_not == true
  end
end

describe "Gravity" do
  before :each do
    @world = World.new
  end

  it "should move downward" do
    body = Body.new 1.0, 0.0, 0.0
    @world << body

    # update by 1 second - initially since the body is at rest, it shouldn't move
    @world.update 1000
    body.x.should == 0.0
    body.y.should == 0.0
    body.vx.should == 0.0
    body.vy.should == -@world.gravity

    # update by 1 second, should move down
    @world.update 1000
    body.x.should == 0.0
    body.y.should == -@world.gravity
    body.vx.should == 0.0
    body.vy.should == -@world.gravity * 2
  end

  it "should not move" do
    # mass-less particles shouldn't move
    body = Body.new 0.0, 0.0, 0.0
    @world << body

    @world.update 1000

    body.x.should == 0.0
    body.y.should == 0.0
    body.vx.should == 0.0
    body.vy.should == 0.0
  end
end
