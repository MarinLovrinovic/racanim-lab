using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UIElements;

public class Boid : MonoBehaviour
{
    private static Vector2 spaceSize = new(16, 9);
    private static Vector2 spaceTopRight = new(8, 4.5f);
    private static Vector2 spaceBottomLeft = new(-8, -4.5f);
    
    [SerializeField] private float desiredSpeed = 4;
    [SerializeField] private float maxAcceleration = 15;
    [SerializeField] private float visionRadius = 4;
    [SerializeField] private float viewAngle = 135;

    private List<Boid> others;
    
    private Vector2 velocity;
    
    public float separationFactor;
    public float alignmentFactor;
    public float cohesionFactor;
    public float obstacleAvoidanceFactor;
    
    void Start()
    {
        velocity = Random.insideUnitCircle.normalized * desiredSpeed;
        GetComponent<SpriteRenderer>().color = Random.ColorHSV(0.57f, 0.6f, 0.5f, 0.7f, 0.7f, 1f);
    }

    public void Init(List<Boid> others)
    {
        this.others = others;
    }

    void FixedUpdate()
    {
        Vector2 separationAcceleration = Vector2.zero;
        
        Vector2 neighborVelocitiesSum = Vector2.zero;
        Vector2 neighborPositionsSum = Vector2.zero;
        float neighborWeightsSum = 0;
        foreach (Boid other in others)
        {
            if (other == this) continue;

            float distance = Vector2.Distance(transform.position, other.transform.position);
            if (distance > visionRadius) continue;

            Vector2 otherDir = (other.transform.position - transform.position).normalized;
            float otherAngle = Vector2.Angle(velocity, otherDir);
            if (otherAngle > viewAngle) continue;

            float weight = 1 / distance;
            
            separationAcceleration += -2 * weight * otherDir;
            
            neighborPositionsSum += (Vector2) other.transform.position * weight;
            neighborVelocitiesSum += other.velocity * weight;
            neighborWeightsSum += weight;
        }
        separationAcceleration.Normalize();
        
        Vector2 averageNeighborPosition = neighborPositionsSum / neighborWeightsSum;
        Vector2 cohesionAcceleration = (averageNeighborPosition - (Vector2)transform.position).normalized;
        
        Vector2 averageNeighborVelocity = neighborVelocitiesSum / neighborWeightsSum;
        Vector2 alignmentAcceleration = (averageNeighborVelocity - velocity).normalized;
        
        Vector2 acceleration = separationFactor * separationAcceleration 
                               + alignmentFactor * alignmentAcceleration
                               + cohesionFactor * cohesionAcceleration
                               + obstacleAvoidanceFactor * ObstacleAvoidance();
        acceleration =
            Vector2.Perpendicular(velocity) * Vector2.Dot(Vector2.Perpendicular(velocity), acceleration);

        if (acceleration.magnitude > maxAcceleration)
            acceleration = acceleration.normalized * maxAcceleration;
        
        velocity += Time.fixedDeltaTime * acceleration;
        velocity = velocity.normalized * desiredSpeed;
        
        transform.position += (Vector3) velocity * Time.fixedDeltaTime;;
        transform.rotation = Quaternion.Euler(0, 0, Mathf.Rad2Deg * Mathf.Atan2(velocity.y, velocity.x) - 90);
        
        ConstrainPosition();
    }

    void ConstrainPosition()
    {
        if (spaceTopRight.x < transform.position.x)
            transform.position += Vector3.left * spaceSize.x;
        if (spaceTopRight.y < transform.position.y)
            transform.position += Vector3.down * spaceSize.y;
        if (transform.position.x < spaceBottomLeft.x)
            transform.position += Vector3.right * spaceSize.x;
        if (transform.position.y < spaceBottomLeft.y)
            transform.position += Vector3.up * spaceSize.y;
    }

    Vector2 ObstacleAvoidance()
    {
        Vector2 totalAcceleration = Vector2.zero;
        Vector2 origin = transform.position;
        float startingAngle = Mathf.Rad2Deg * Mathf.Atan2(velocity.y, velocity.x) - 90;
        for (int i = 0; i < 10; i++)
        {
            float angle = startingAngle + i * 20f;
            angle *= Mathf.Deg2Rad;
            Vector2 direction = new Vector2(Mathf.Cos(angle), Mathf.Sin(angle));

            RaycastHit2D hit = Physics2D.Raycast(origin, direction, visionRadius);
            if (hit && hit.distance > 0)
            {
                totalAcceleration += -2 * direction / (hit.distance / 2);
            }
        }
        return totalAcceleration;
    }
}
