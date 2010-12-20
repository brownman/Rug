require File.dirname(__FILE__) + '/../lib/Physics'

include Rug
include Rug::Physics

describe "Collision detection" do
  before :all do
    @body1 = Body.new nil, 0.0, [20, 20]
    @body2 = Body.new nil, 0.0, [15, 15]
    @body3 = Body.new nil, 0.0, [100, 100]

    @rect = Rectangle.new @body1, 10, 10
    @circ = Circle.new @body2, 10
    @circ2 = Circle.new @body3, 10
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
    body = Body.new @world, 1.0, [0.0, 0.0]

    # update by 1 second - initially since the body is at rest, it shouldn't move
    @world.update 1000
    body.x.should == 0.0
    body.y.should == 0.0
    body.vx.should == 0.0
    body.vy.should == -9.8

    # update by 1 second, should move down
    @world.update 1000
    body.x.should == 0.0
    body.y.should == -9.8
    body.vx.should == 0.0
    body.vy.should == -9.8 * 2
  end

  it "should not move" do
    # mass-less particles shouldn't move
    body = Body.new @world, 0.0, [0.0, 0.0]

    @world.update 1000

    body.x.should == 0.0
    body.y.should == 0.0
    body.vx.should == 0.0
    body.vy.should == 0.0
  end
end